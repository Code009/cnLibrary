/*- cnUI - Image Control --------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2019-12-06                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_cnUI_ImageControl_H__
#define __cnLib_cnUI_ImageControl_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#include <cnUI/BasicControl.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
cnLib_ENUM_BEGIN(ufInt8,ImageScaleType)
{
	None,
	Aspect,
	AspectFull,
	StrectFull,
}cnLib_ENUM_END(ImageScaleType);
//---------------------------------------------------------------------------
bool ImageRatioCompare(cUIPoint Size1,cUIPoint Size2);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viImageData : public viData
{
public:
	cFunctionSet<void (void)> ControlImageNotifySet;
	virtual iBitmapDataSource* ControlImage(void)=0;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)=0;
	virtual eAlignment ControlImageAlignment(void)=0;
	virtual eImageScaleType ControlImageScale(void)=0;
};
//---------------------------------------------------------------------------
class vImage : public vSimpleViewPainter
{
public:
	vImage(viImageData *Data=nullptr);
	~vImage();

	static rPtr<viControl> Create(viImageData *Data);

	viImageData* GetData(void);
	void SetData(viImageData *Data);

protected:

	dPtr<viImageData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
private:
	void Update(void);

	iPtr<iUIBitmap> fCacheImageGraph;
	cUIPoint fImageGraphSize;
	bool fUpdateImage;

	void SetupImageCache(void);

	cUIPoint TransformImageRect(void);

	iFunctionToken *fImageDataNotifyToken;

	void DataInsertCallback(void);
	void DataRemoveCallback(void);
};
//---------------------------------------------------------------------------
class cImage : public cVisual, public viImageData
{ 
public:
	cImage();
	~cImage();

	void Update(void);

	iPtr<iBitmapDataSource> Image;
	eAlignment ImageAlign=Alignment::Center;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;
	eImageScaleType ImageScale=ImageScaleType::AspectFull;

	virtual iBitmapDataSource* ControlImage(void)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)override;
	virtual eAlignment ControlImageAlignment(void)override;
	virtual eImageScaleType ControlImageScale(void)override;


protected:

	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viButtonImageData : public viData
{
public:
	cFunctionSet<void (void)> ButtonImageNotifySet;
	virtual iBitmapDataSource* ButtonImageForState(eButtonState State)=0;
	virtual eImageBlendingOperator ButtonImageBlendingOperatorForState(eButtonState State)=0;
	virtual eAlignment ButtonImageAlignment(void)=0;
};
//---------------------------------------------------------------------------
class cButtonImageData : public viButtonImageData
{
public:
	cButtonImageData();
	~cButtonImageData();

	eAlignment ImageAlign=Alignment::None;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;
	iPtr<iBitmapDataSource> Image;
	iPtr<iBitmapDataSource> HotImage;
	iPtr<iBitmapDataSource> PressedImage;
	iPtr<iBitmapDataSource> DisabledImage;
	iPtr<iBitmapDataSource> InactiveImage;

	void Update(void);

	virtual iBitmapDataSource* ButtonImageForState(eButtonState State)override;
	virtual eImageBlendingOperator ButtonImageBlendingOperatorForState(eButtonState State)override;
	virtual eAlignment ButtonImageAlignment(void)override;
protected:
};
//---------------------------------------------------------------------------
class vButtonImage : public vSimpleViewPainter
{
public:
	vButtonImage(viButtonData *ButtonData=nullptr,viButtonImageData *ImageData=nullptr);
	~vButtonImage();

	static rPtr<viControl> Create(viButtonData *ButtonData,viButtonImageData *ImageData);

	viButtonData* GetButtonData(void)const;
	void SetButtonData(viButtonData *Data);
	viButtonImageData* GetImageData(void)const;
	void SetImageData(viButtonImageData *Data);
	void SetData(viButtonData *ButtonData,viButtonImageData *ImageData);

protected:
	dPtr<viButtonData> fButtonData;
	dPtr<viButtonImageData> fImageData;

	iPtr<iUIBitmap> fCacheImageNormal;
	iPtr<iUIBitmap> fCacheImageHot;
	iPtr<iUIBitmap> fCacheImagePressed;
	iPtr<iUIBitmap> fCacheImageInactive;
	iPtr<iUIBitmap> fCacheImageDisabled;

	void Update(void);
	void UpdateState(void);

	void ClearCache(void);
	void SetupCacheNormal(void);
	void SetupCacheImage(iPtr<iUIBitmap> &CacheImage,eButtonState State);
	void SetupCacheHot(void);
	void SetupCachePressed(void);
	void SetupCacheInactive(void);
	void SetupCacheDisabled(void);

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
private:
	iFunctionToken *fButtonDataNofityToken;
	iFunctionToken *fButtonImageDataNotifyToken;

	void ButtonDataInsertCallback(void);
	void ButtonDataRemoveCallback(void);
	void ImageDataInsertCallback(void);
	void ImageDataRemoveCallback(void);
};
//---------------------------------------------------------------------------
struct cLayoutImageTextOptions
{
	cUIRectangle Margin;
	sfInt16 ImageMargin;
	eAlignment TextAlign;
	eAroundAlignment ImageAlign;
};
//---------------------------------------------------------------------------
void LayoutImageText(cUIPoint &TextPos,cUIPoint &ImagePos,const cUIPoint &TextSize,const cUIPoint &ImageSize,
	const cUIPoint &AreaSize,const cLayoutImageTextOptions &Option);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viImageTextControlData : public viData
{
public:
	cFunctionSet<void (void)> ControlImageTextNotifySet;
	virtual const uChar16* ControlTextString(void)=0;
	virtual uIntn ControlTextLength(void)=0;
	virtual const cUITextStyle& ControlTextStyle(void)=0;
	virtual iBitmapDataSource* ControlImage(void)=0;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)=0;
	virtual cLayoutImageTextOptions ControlImageTextOption(void)=0;
};
//---------------------------------------------------------------------------
class vImageLabel : public vSimpleViewPainter
{
public:
	vImageLabel(viImageTextControlData *Data=nullptr);
	~vImageLabel();

	static rPtr<viControl> Create(viImageTextControlData *Data);

	viImageTextControlData* GetData(void)const;
	void SetData(viImageTextControlData *Data);

protected:

	dPtr<viImageTextControlData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)override;
private:

	iPtr<iUIBitmap> fCacheImageGraph;
	iPtr<iUITextGraph> fCacheTextGraph;
	cUIPoint fImageGraphSize;
	cUIPoint fTextGraphSize;
	bool fUpdateText;
	bool fUpdateImage;

	void SetupTextCache(void);
	void SetupImageCache(void);

	void Update(void);

	void DataInsertCallback(void);
	void DataRemoveCallback(void);
	
	iFunctionToken *fImageTextNoftifyToken;
};
//---------------------------------------------------------------------------
class cImageLabel : public cVisual, public viImageTextControlData
{ 
public:
	cImageLabel();
	~cImageLabel();

	void Update(void);

	iPtr<iBitmapDataSource> Image;
	cString<uChar16> Text;
	cUITextStyle TextStyle;
	eAlignment TextAlign=Alignment::CenterLeft;
	eAroundAlignment ImageAlign=AroundAlignment::HCenterLeft;
	sfInt8 ImageMargin=2;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;

	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	virtual const cUITextStyle& ControlTextStyle(void)override;
	virtual iBitmapDataSource* ControlImage(void)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)override;
	virtual cLayoutImageTextOptions ControlImageTextOption(void)override;

protected:
	
	virtual void ControlContentSetDefault(void)override;
};
//---------------------------------------------------------------------------
class bcImageTextButton : public bcButton, public viImageTextControlData
{
public:
	bcImageTextButton();
	~bcImageTextButton();

	cUITextStyle TextStyle;

	Float32 ContentZPosition=ZPosition_Content;

	sfInt8 ContentMarginLeft=0;
	sfInt8 ContentMarginTop=0;
	sfInt8 ContentMarginRight=0;
	sfInt8 ContentMarginBottom=0;

	eAlignment TextAlign=Alignment::Center;
	eAroundAlignment ImageAlign=AroundAlignment::HCenterLeft;
	sfInt8 ImageMargin=2;


	void SetContent(rPtr<viControl> Content);
	void UpdateZPosition(void);

	void Update(void);
	void UpdateState(void);

	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	virtual const cUITextStyle& ControlTextStyle(void)override;
	virtual iBitmapDataSource* ControlImage(void)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)override;
	virtual cLayoutImageTextOptions ControlImageTextOption(void)override;

protected:

	virtual void ViewSetup(void)override;
	virtual void ViewClear(void)override;

	rPtr<viControl> fContent;
	virtual void ControlContentSetDefault(void);
};
//---------------------------------------------------------------------------
class cImageTextButton : public bcImageTextButton
{
public:
	cImageTextButton();
	~cImageTextButton();

	iPtr<iBitmapDataSource> Image;
	cString<uChar16> Text;
	ufInt8 Check=0;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;

	virtual ufInt8 CheckState(void)override;
	virtual const uChar16* ControlTextString(void)override;
	virtual uIntn ControlTextLength(void)override;
	virtual iBitmapDataSource* ControlImage(void)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)override;
	
	cFunction<void (void)> OnClick;

protected:
	virtual bool ButtonMouseAllowButton(eMouseButton Button)override;
	virtual void ButtonClick(const cUIPoint &Pos)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
