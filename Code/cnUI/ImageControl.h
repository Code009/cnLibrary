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
bool ImageRatioCompare(cUIPoint Size1,cUIPoint Size2)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viImageData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlImageNotifySet;
	virtual iBitmapDataSource* ControlImage(void)noexcept(true)=0;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)noexcept(true)=0;
	virtual eAlignment ControlImageAlignment(void)noexcept(true)=0;
	virtual eImageScaleType ControlImageScale(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vImage : public vSimpleViewPainter
{
public:
	vImage(viImageData *Data=nullptr)noexcept(true);
	~vImage()noexcept(true);

	static rPtr<viControl> Create(viImageData *Data)noexcept(true);

	viImageData* GetData(void)noexcept(true);
	void SetData(viImageData *Data)noexcept(true);

protected:

	dPtr<viImageData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
private:
	void Update(void)noexcept(true);

	iPtr<iUIBitmap> fCacheImageGraph;
	cUIPoint fImageGraphSize;
	bool fUpdateImage;

	void SetupImageCache(void)noexcept(true);

	cUIPoint TransformImageRect(void)noexcept(true);

	iFunctionToken *fImageDataNotifyToken;

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cImage : public cVisual, public viImageData
{ 
public:
	cImage()noexcept(true);
	~cImage()noexcept(true);

	void Update(void)noexcept(true);

	iPtr<iBitmapDataSource> Image;
	eAlignment ImageAlign=Alignment::Center;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;
	eImageScaleType ImageScale=ImageScaleType::AspectFull;

	virtual iBitmapDataSource* ControlImage(void)noexcept(true)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)noexcept(true)override;
	virtual eAlignment ControlImageAlignment(void)noexcept(true)override;
	virtual eImageScaleType ControlImageScale(void)noexcept(true)override;


protected:

	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viButtonImageData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ButtonImageNotifySet;
	virtual iBitmapDataSource* ButtonImageForState(eButtonState State)noexcept(true)=0;
	virtual eImageBlendingOperator ButtonImageBlendingOperatorForState(eButtonState State)noexcept(true)=0;
	virtual eAlignment ButtonImageAlignment(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cButtonImageData : public viButtonImageData
{
public:
	cButtonImageData()noexcept(true);
	~cButtonImageData()noexcept(true);

	eAlignment ImageAlign=Alignment::None;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;
	iPtr<iBitmapDataSource> Image;
	iPtr<iBitmapDataSource> HotImage;
	iPtr<iBitmapDataSource> PressedImage;
	iPtr<iBitmapDataSource> DisabledImage;
	iPtr<iBitmapDataSource> InactiveImage;

	void Update(void)noexcept(true);

	virtual iBitmapDataSource* ButtonImageForState(eButtonState State)noexcept(true)override;
	virtual eImageBlendingOperator ButtonImageBlendingOperatorForState(eButtonState State)noexcept(true)override;
	virtual eAlignment ButtonImageAlignment(void)noexcept(true)override;
protected:
};
//---------------------------------------------------------------------------
class vButtonImage : public vSimpleViewPainter
{
public:
	vButtonImage(viButtonData *ButtonData=nullptr,viButtonImageData *ImageData=nullptr)noexcept(true);
	~vButtonImage()noexcept(true);

	static rPtr<viControl> Create(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept(true);

	viButtonData* GetButtonData(void)const noexcept(true);
	void SetButtonData(viButtonData *Data)noexcept(true);
	viButtonImageData* GetImageData(void)const noexcept(true);
	void SetImageData(viButtonImageData *Data)noexcept(true);
	void SetData(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept(true);

protected:
	dPtr<viButtonData> fButtonData;
	dPtr<viButtonImageData> fImageData;

	iPtr<iUIBitmap> fCacheImageNormal;
	iPtr<iUIBitmap> fCacheImageHot;
	iPtr<iUIBitmap> fCacheImagePressed;
	iPtr<iUIBitmap> fCacheImageInactive;
	iPtr<iUIBitmap> fCacheImageDisabled;

	void Update(void)noexcept(true);
	void UpdateState(void)noexcept(true);

	void ClearCache(void)noexcept(true);
	void SetupCacheNormal(void)noexcept(true);
	void SetupCacheImage(iPtr<iUIBitmap> &CacheImage,eButtonState State)noexcept(true);
	void SetupCacheHot(void)noexcept(true);
	void SetupCachePressed(void)noexcept(true);
	void SetupCacheInactive(void)noexcept(true);
	void SetupCacheDisabled(void)noexcept(true);

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
private:
	iFunctionToken *fButtonDataNofityToken;
	iFunctionToken *fButtonImageDataNotifyToken;

	void ButtonDataInsertCallback(void)noexcept(true);
	void ButtonDataRemoveCallback(void)noexcept(true);
	void ImageDataInsertCallback(void)noexcept(true);
	void ImageDataRemoveCallback(void)noexcept(true);
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
	const cUIPoint &AreaSize,const cLayoutImageTextOptions &Option)noexcept(true);
//---------------------------------------------------------------------------
class cnLib_INTERFACE viImageTextControlData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlImageTextNotifySet;
	virtual const uChar16* ControlTextString(void)noexcept(true)=0;
	virtual uIntn ControlTextLength(void)noexcept(true)=0;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)=0;
	virtual iBitmapDataSource* ControlImage(void)noexcept(true)=0;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)noexcept(true)=0;
	virtual cLayoutImageTextOptions ControlImageTextOption(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class vImageLabel : public vSimpleViewPainter
{
public:
	vImageLabel(viImageTextControlData *Data=nullptr)noexcept(true);
	~vImageLabel()noexcept(true);

	static rPtr<viControl> Create(viImageTextControlData *Data)noexcept(true);

	viImageTextControlData* GetData(void)const noexcept(true);
	void SetData(viImageTextControlData *Data)noexcept(true);

protected:

	dPtr<viImageTextControlData> fData;

	virtual void cnLib_FUNC Paint(iUISimplePaintContext *Context)noexcept(true)override;
private:

	iPtr<iUIBitmap> fCacheImageGraph;
	iPtr<iUITextGraph> fCacheTextGraph;
	cUIPoint fImageGraphSize;
	cUIPoint fTextGraphSize;
	bool fUpdateText;
	bool fUpdateImage;

	void SetupTextCache(void)noexcept(true);
	void SetupImageCache(void)noexcept(true);

	void Update(void)noexcept(true);

	void DataInsertCallback(void)noexcept(true);
	void DataRemoveCallback(void)noexcept(true);
	
	iFunctionToken *fImageTextNoftifyToken;
};
//---------------------------------------------------------------------------
class cImageLabel : public cVisual, public viImageTextControlData
{ 
public:
	cImageLabel()noexcept(true);
	~cImageLabel()noexcept(true);

	void Update(void)noexcept(true);

	iPtr<iBitmapDataSource> Image;
	cString<uChar16> Text;
	cUITextStyle TextStyle;
	eAlignment TextAlign=Alignment::CenterLeft;
	eAroundAlignment ImageAlign=AroundAlignment::HCenterLeft;
	sfInt8 ImageMargin=2;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;

	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)override;
	virtual iBitmapDataSource* ControlImage(void)noexcept(true)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)noexcept(true)override;
	virtual cLayoutImageTextOptions ControlImageTextOption(void)noexcept(true)override;

protected:
	
	virtual void ControlContentSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class bcImageTextButton : public bcButton, public viImageTextControlData
{
public:
	bcImageTextButton()noexcept(true);
	~bcImageTextButton()noexcept(true);

	cUITextStyle TextStyle;

	Float32 ContentZPosition=ZPosition_Content;

	sfInt8 ContentMarginLeft=0;
	sfInt8 ContentMarginTop=0;
	sfInt8 ContentMarginRight=0;
	sfInt8 ContentMarginBottom=0;

	eAlignment TextAlign=Alignment::Center;
	eAroundAlignment ImageAlign=AroundAlignment::HCenterLeft;
	sfInt8 ImageMargin=2;


	void SetContent(rPtr<viControl> Content)noexcept(true);
	void UpdateZPosition(void)noexcept(true);

	void Update(void)noexcept(true);
	void UpdateState(void)noexcept(true);

	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	virtual const cUITextStyle& ControlTextStyle(void)noexcept(true)override;
	virtual iBitmapDataSource* ControlImage(void)noexcept(true)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)noexcept(true)override;
	virtual cLayoutImageTextOptions ControlImageTextOption(void)noexcept(true)override;

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	rPtr<viControl> fContent;
	virtual void ControlContentSetDefault(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cImageTextButton : public bcImageTextButton
{
public:
	cImageTextButton()noexcept(true);
	~cImageTextButton()noexcept(true);

	iPtr<iBitmapDataSource> Image;
	cString<uChar16> Text;
	ufInt8 Check=0;
	eImageBlendingOperator ImageBlend=ImageBlendingOperator::Alpha;

	virtual ufInt8 CheckState(void)noexcept(true)override;
	virtual const uChar16* ControlTextString(void)noexcept(true)override;
	virtual uIntn ControlTextLength(void)noexcept(true)override;
	virtual iBitmapDataSource* ControlImage(void)noexcept(true)override;
	virtual eImageBlendingOperator ControlImageBlendingOperator(void)noexcept(true)override;
	
	cFunction<void (void)noexcept(true)> OnClick;

protected:
	virtual bool ButtonMouseAllowButton(eMouseButton Button)noexcept(true)override;
	virtual void ButtonClick(const cUIPoint &Pos)noexcept(true)override;
};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
