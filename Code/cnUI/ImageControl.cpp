#include "ImageControl.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnUI::ImageRatioCompare(cUIPoint Size1,cUIPoint Size2)noexcept
{
	return Size1.x*Size2.y < Size1.y*Size2.x;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vImage::vImage(viImageData *Data)noexcept
	: fData(Data)
{
	fUpdateImage=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vImage::~vImage()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vImage::Create(viImageData *Data)noexcept
{
	return rCreate< bwvControl<vImage> >(Data);
}
//---------------------------------------------------------------------------
viImageData* vImage::GetData(void)noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vImage::SetData(viImageData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vImage::Update(void)noexcept
{
	fUpdateImage=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vImage::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fImageDataNotifyToken=fData->ControlImageNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vImage::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->ControlImageNotifySet.Remove(fImageDataNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vImage::Paint(iUISimplePaintContext *Context)noexcept
{
	SetupImageCache();

	if(fCacheImageGraph==nullptr)
		return;


	if(fCacheImageGraph!=nullptr){
		auto BlendOp=fData->ControlImageBlendingOperator();
		cUIPoint DrawImagePos=TransformImageRect();
		Context->Image(DrawImagePos,fImageGraphSize,fCacheImageGraph,BlendOp);
	}
}
//---------------------------------------------------------------------------
cUIPoint vImage::TransformImageRect()noexcept
{
	cUIPoint ImageSize=fImageGraphSize;
	cUIPoint AreaSize=fPaintSize;
	auto ImageScale=fData->ControlImageScale();
	switch(ImageScale){
	default:
	case ImageScaleType::None:
		break;
	case ImageScaleType::Aspect:
		if(ImageRatioCompare(ImageSize,AreaSize)){
			ImageSize.x=ImageSize.x*AreaSize.y/ImageSize.y;
			ImageSize.y=AreaSize.y;
		}
		else{
			ImageSize.y=ImageSize.y*AreaSize.x/ImageSize.x;
			ImageSize.x=AreaSize.x;
		}
		break;
	case ImageScaleType::AspectFull:
		if(ImageRatioCompare(ImageSize,AreaSize)){
			ImageSize.y=ImageSize.y*AreaSize.x/ImageSize.x;
			ImageSize.x=AreaSize.x;
		}
		else{
			ImageSize.x=ImageSize.x*AreaSize.y/ImageSize.y;
			ImageSize.y=AreaSize.y;
		}
		break;
	case ImageScaleType::StrectFull:
		ImageSize=AreaSize;
		return UIPointZero;
	}
	auto ImageAlign=fData->ControlImageAlignment();
	cUIPoint TextPos=RectStretchAlignInRect(ImageAlign,AreaSize,ImageSize);
	return TextPos;
}
//---------------------------------------------------------------------------
void vImage::SetupImageCache(void)noexcept
{
	if(fUpdateImage==false){
		return;
	}
	fUpdateImage=false;
	fCacheImageGraph=nullptr;
	fImageGraphSize={0,0};

	if(fData==nullptr)
		return;

	auto ImageSource=fData->ControlImage();
	if(ImageSource==nullptr){
		return;
	}
	auto Device=fViewContent->QueryDevice();
	fCacheImageGraph=Device->CreateBitmapCopyFromSource(ImageSource);

	if(fCacheImageGraph==nullptr)
		return;

	fImageGraphSize=fCacheImageGraph->GetSize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cImage::cImage()noexcept
{
}
//---------------------------------------------------------------------------
cImage::~cImage()noexcept
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cImage::ControlContentSetDefault(void)noexcept
{
	SetContent(vImage::Create(this));
}
//---------------------------------------------------------------------------
void cImage::Update(void)noexcept
{
	ControlImageNotifySet();
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImage::ControlImage(void)noexcept
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImage::ControlImageBlendingOperator(void)noexcept
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
eAlignment cImage::ControlImageAlignment(void)noexcept
{
	return ImageAlign;
}
//---------------------------------------------------------------------------
eImageScaleType cImage::ControlImageScale(void)noexcept
{
	return ImageScale;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cButtonImageData::cButtonImageData()noexcept
{
}
//---------------------------------------------------------------------------
cButtonImageData::~cButtonImageData()noexcept
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cButtonImageData::Update(void)noexcept
{
	ButtonImageNotifySet();
}
//---------------------------------------------------------------------------
iBitmapDataSource* cButtonImageData::ButtonImageForState(eButtonState State)noexcept
{
	switch(State){
	case ButtonState::Normal:
		return Image;
	case ButtonState::Hot:
		return HotImage;
	case ButtonState::Pressed:
		return PressedImage;
	case ButtonState::Disabled:
		return DisabledImage;
	case ButtonState::Inactive:
		return InactiveImage;
	}
	return nullptr;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cButtonImageData::ButtonImageBlendingOperatorForState(eButtonState)noexcept
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
eAlignment cButtonImageData::ButtonImageAlignment(void)noexcept
{
	return ImageAlign;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vButtonImage::vButtonImage(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept
	: fButtonData(ButtonData)
	, fImageData(ImageData)
{
	ButtonDataInsertCallback();
	ImageDataInsertCallback();
}
//---------------------------------------------------------------------------
vButtonImage::~vButtonImage()noexcept
{
	ButtonDataRemoveCallback();
	ImageDataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vButtonImage::Create(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept
{
	return rCreate< bwvControl<vButtonImage> >(ButtonData,ImageData);
}
//---------------------------------------------------------------------------
viButtonData* vButtonImage::GetButtonData(void)const noexcept
{
	return fButtonData;
}
//---------------------------------------------------------------------------
void vButtonImage::SetButtonData(viButtonData *Data)noexcept
{
	if(fButtonData==Data)
		return;

	ButtonDataRemoveCallback();
	fButtonData=Data;
	ButtonDataInsertCallback();

	UpdateState();
}
//---------------------------------------------------------------------------
viButtonImageData* vButtonImage::GetImageData(void)const noexcept
{
	return fImageData;
}
//---------------------------------------------------------------------------
void vButtonImage::SetImageData(viButtonImageData *Data)noexcept
{
	if(fImageData==Data)
		return;

	ImageDataRemoveCallback();
	fImageData=Data;
	ImageDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vButtonImage::SetData(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept
{
	if(fButtonData==ButtonData)
		return SetImageData(ImageData);
	if(fImageData==ImageData)
		return SetButtonData(ButtonData);

	ButtonDataRemoveCallback();
	ImageDataRemoveCallback();
	fButtonData=ButtonData;
	fImageData=ImageData;
	ButtonDataInsertCallback();
	ImageDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vButtonImage::ButtonDataInsertCallback(void)noexcept
{
	if(fButtonData!=nullptr){
		fButtonDataNofityToken=fButtonData->ButtonNotifySet.Insert([this]{
			UpdateState();
		});
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ButtonDataRemoveCallback(void)noexcept
{
	if(fButtonData!=nullptr){
		fButtonData->ButtonNotifySet.Remove(fButtonDataNofityToken);
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ImageDataInsertCallback(void)noexcept
{
	if(fImageData!=nullptr){
		fButtonDataNofityToken=fImageData->ButtonImageNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ImageDataRemoveCallback(void)noexcept
{
	if(fImageData!=nullptr){
		fImageData->ButtonImageNotifySet.Remove(fButtonDataNofityToken);
	}
}
//---------------------------------------------------------------------------
void vButtonImage::Paint(iUISimplePaintContext *Context)noexcept
{
	if(fImageData==nullptr)
		return;
	eButtonState BtnState;
	if(fButtonData!=nullptr){
		BtnState=fButtonData->ButtonState();
	}
	else{
		BtnState=ButtonState::Disabled;
	}
	// draw button image
	iUIBitmap *DrawImage=nullptr;
	switch(BtnState){
	case ButtonState::Normal:
		SetupCacheNormal();
		DrawImage=fCacheImageNormal;
		break;
	case ButtonState::Hot:
		SetupCacheHot();
		DrawImage=fCacheImageHot;
		break;
	case ButtonState::Pressed:
		SetupCachePressed();
		DrawImage=fCacheImagePressed;
		break;
	case ButtonState::Inactive:
		SetupCacheInactive();
		DrawImage=fCacheImageInactive;
		break;
	case ButtonState::Disabled:
		SetupCacheDisabled();
		DrawImage=fCacheImageDisabled;
		break;
	}
	if(DrawImage!=nullptr){
		cUIPoint DrawImageSize=DrawImage->GetSize();
		auto ImageAlign=fImageData->ButtonImageAlignment();
		cUIPoint DrawImagePos=RectStretchAlignInRect(ImageAlign,fPaintSize,DrawImageSize);
			
		auto BlendOp=fImageData->ButtonImageBlendingOperatorForState(BtnState);
		Context->Image(DrawImagePos,DrawImageSize,DrawImage,BlendOp);
	}

}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheImage(iPtr<iUIBitmap> &CacheImage,eButtonState State)noexcept
{
	auto ImageSource=fImageData->ButtonImageForState(State);
	if(ImageSource==nullptr){
		return;
	}

	auto Device=fViewContent->QueryDevice();
	CacheImage=Device->CreateBitmapCopyFromSource(ImageSource);
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheNormal(void)noexcept
{
	SetupCacheImage(fCacheImageNormal,ButtonState::Normal);
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheHot(void)noexcept
{
	SetupCacheImage(fCacheImageHot,ButtonState::Hot);
	if(fCacheImageHot==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageHot=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCachePressed(void)noexcept
{
	SetupCacheImage(fCacheImagePressed,ButtonState::Pressed);
	if(fCacheImagePressed==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImagePressed=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheInactive(void)noexcept
{
	SetupCacheImage(fCacheImageInactive,ButtonState::Inactive);
	if(fCacheImageInactive==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageInactive=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheDisabled(void)noexcept
{
	SetupCacheImage(fCacheImageDisabled,ButtonState::Disabled);
	if(fCacheImageDisabled==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageDisabled=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ClearCache(void)noexcept
{
	fCacheImageNormal=nullptr;
	fCacheImageHot=nullptr;
	fCacheImagePressed=nullptr;
	fCacheImageDisabled=nullptr;
	fCacheImageInactive=nullptr;
}
//---------------------------------------------------------------------------
void vButtonImage::Update(void)noexcept
{
	ClearCache();
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vButtonImage::UpdateState(void)noexcept
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnUI::LayoutImageText(cUIPoint &TextPos,cUIPoint &ImagePos,const cUIPoint &TextSize,const cUIPoint &ImageSize,const cUIPoint &AreaSize,const cLayoutImageTextOptions &Option)noexcept
{
	cUIPoint ContentAreaSize=AreaSize;
	ContentAreaSize.x-=Option.Margin.Left+Option.Margin.Right;
	ContentAreaSize.y-=Option.Margin.Top+Option.Margin.Bottom;
	cUIPoint TotalSize=TextSize;

	switch(Option.ImageAlign&AroundAlignment::MaskDirection){
	default:
		break;
	case AroundAlignment::Left:
		ImagePos.x=0;
		TextPos.x=ImageSize.x+Option.ImageMargin;
		TotalSize.x+=ImageSize.x+Option.ImageMargin;
		break;
	case AroundAlignment::Right:
		TextPos.x=0;
		ImagePos.x=TextSize.x+Option.ImageMargin;
		TotalSize.x+=ImageSize.x+Option.ImageMargin;
		break;
	case AroundAlignment::Top:
		ImagePos.y=0;
		TextPos.y=ImageSize.y+Option.ImageMargin;
		TotalSize.y+=ImageSize.y+Option.ImageMargin;
		break;
	case AroundAlignment::Bottom:
		TextPos.y=0;
		ImagePos.y=TextSize.y+Option.ImageMargin;
		TotalSize.y+=ImageSize.y+Option.ImageMargin;
		break;
	}

	if((Option.ImageAlign&AroundAlignment::MaskAxis)==AroundAlignment::Horizontal){
		if(TotalSize.y<ImageSize.y){
			TotalSize.y=ImageSize.y;
		}
		switch(Option.ImageAlign&AroundAlignment::MaskSide){
		case AroundAlignment::SideLowwer:
			// Top
			ImagePos.y=0;
			TextPos.y=0;
			break;
		case AroundAlignment::SideUpper:
			// Bottom
			TextPos.y=TotalSize.y-TextSize.y;
			ImagePos.y=TotalSize.y-ImageSize.y;
			break;
		default:
		case AroundAlignment::SideMiddle:
			// V-Center
			if(TextSize.y<ImageSize.y){
				ImagePos.y=0;
				TextPos.y=(ImageSize.y-TextSize.y)/2;
			}
			else{
				TextPos.y=0;
				ImagePos.y=(TextSize.y-ImageSize.y)/2;
			}
			break;
		}
	}
	else{
		if(TotalSize.x<ImageSize.x){
			TotalSize.x=ImageSize.x;
		}
		switch(Option.ImageAlign&AroundAlignment::MaskSide){
		case AroundAlignment::SideLowwer:
			// Left
			ImagePos.x=0;
			TextPos.x=0;
			break;
		case AroundAlignment::SideUpper:
			// Right
			TextPos.x=TotalSize.x-TextSize.x;
			ImagePos.x=TotalSize.x-ImageSize.x;
			break;
		default:
		case AroundAlignment::SideMiddle:
			// H-Center
			if(TotalSize.x<ImageSize.x){
				ImagePos.x=0;
				TextPos.x=(ImageSize.x-TextSize.x)/2;
			}
			else{
				TextPos.x=0;
				ImagePos.x=(TextSize.x-ImageSize.x)/2;
			}
			break;
		}
	}

	// calculate box rectangle
	cUIPoint DrawPos=RectAlignInRect(Option.TextAlign,ContentAreaSize,TotalSize);

	TextPos.x+=Option.Margin.Left+DrawPos.x;
	ImagePos.x+=Option.Margin.Left+DrawPos.x;
	TextPos.y+=Option.Margin.Top+DrawPos.y;
	ImagePos.y+=Option.Margin.Top+DrawPos.y;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vImageLabel::vImageLabel(viImageTextControlData *Data)noexcept
	: fData(Data)
{
	fUpdateImage=true;
	fUpdateText=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vImageLabel::~vImageLabel()noexcept
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vImageLabel::Create(viImageTextControlData *Data)noexcept
{
	return rCreate< bwvControl<vImageLabel> >(Data);
}
//---------------------------------------------------------------------------
viImageTextControlData* vImageLabel::GetData(void)const noexcept
{
	return fData;
}
//---------------------------------------------------------------------------
void vImageLabel::SetData(viImageTextControlData *Data)noexcept
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vImageLabel::DataInsertCallback(void)noexcept
{
	if(fData!=nullptr){
		fImageTextNoftifyToken=fData->ControlImageTextNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vImageLabel::DataRemoveCallback(void)noexcept
{
	if(fData!=nullptr){
		fData->ControlImageTextNotifySet.Remove(fImageTextNoftifyToken);
	}
}
//---------------------------------------------------------------------------
void vImageLabel::Update(void)noexcept
{
	fUpdateImage=true;
	fUpdateText=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vImageLabel::Paint(iUISimplePaintContext *Context)noexcept
{
	SetupTextCache();
	SetupImageCache();

	if(fCacheTextGraph==nullptr && fCacheImageGraph==nullptr)
		return;


	cLayoutImageTextOptions Option=fData->ControlImageTextOption();

	cUIPoint TextPos;
	cUIPoint ImagePos;
	LayoutImageText(TextPos,ImagePos,fTextGraphSize,fImageGraphSize,fPaintSize,Option);

	if(fCacheImageGraph!=nullptr){
		auto BlendOp=fData->ControlImageBlendingOperator();
		Context->Image(ImagePos,fImageGraphSize,fCacheImageGraph,BlendOp);
	}


	if(fCacheTextGraph!=nullptr){
		// draw text
		Context->Graph(TextPos,fTextGraphSize,fCacheTextGraph);
	}
}
//---------------------------------------------------------------------------
void vImageLabel::SetupTextCache(void)noexcept
{
	if(fUpdateText==false){
		return;
	}
	fUpdateText=false;
	fCacheTextGraph=nullptr;
	fTextGraphSize={0,0};

	if(fData==nullptr)
		return;

	auto Length=fData->ControlTextLength();
	if(Length==0){
		return;
	}
	auto Device=fViewContent->QueryDevice();
	auto String=fData->ControlTextString();
	auto &Style=fData->ControlTextStyle();
	fCacheTextGraph=Device->CreateTextGraph(String,Length,Style,nullptr);
	if(fCacheTextGraph==nullptr)
		return;

	fTextGraphSize=fCacheTextGraph->GetSize();
}
//---------------------------------------------------------------------------
void vImageLabel::SetupImageCache(void)noexcept
{
	if(fUpdateImage==false){
		return;
	}
	fUpdateImage=false;
	fCacheImageGraph=nullptr;
	fImageGraphSize={0,0};

	if(fData==nullptr)
		return;
	auto ImageSource=fData->ControlImage();
	if(ImageSource==nullptr){
		return;
	}
	auto Device=fViewContent->QueryDevice();
	fCacheImageGraph=Device->CreateBitmapCopyFromSource(ImageSource);

	if(fCacheImageGraph==nullptr)
		return;

	fImageGraphSize=fCacheImageGraph->GetSize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cImageLabel::cImageLabel()noexcept
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
cImageLabel::~cImageLabel()noexcept
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cImageLabel::ControlContentSetDefault(void)noexcept
{
	SetContent(vImageLabel::Create(this));
}
//---------------------------------------------------------------------------
void cImageLabel::Update(void)noexcept
{
	ControlImageTextNotifySet();
}
//---------------------------------------------------------------------------
const uChar16* cImageLabel::ControlTextString(void)noexcept
{
	return Text->Pointer;
}
//---------------------------------------------------------------------------
uIntn cImageLabel::ControlTextLength(void)noexcept
{
	return Text->Length;
}
//---------------------------------------------------------------------------
const cUITextStyle& cImageLabel::ControlTextStyle(void)noexcept
{
	return TextStyle;
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImageLabel::ControlImage(void)noexcept
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImageLabel::ControlImageBlendingOperator(void)noexcept
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
cLayoutImageTextOptions cImageLabel::ControlImageTextOption(void)noexcept
{
	cLayoutImageTextOptions Options;
	Options.Margin.Left=0;
	Options.Margin.Top=0;
	Options.Margin.Right=0;
	Options.Margin.Bottom=0;
	Options.ImageMargin=ImageMargin;
	Options.TextAlign=TextAlign;
	Options.ImageAlign=ImageAlign;
	return Options;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcImageTextButton::bcImageTextButton()noexcept
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
bcImageTextButton::~bcImageTextButton()noexcept
{
	SetView(nullptr);
	viImageTextControlData::InvalidateData();
}
//---------------------------------------------------------------------------
void bcImageTextButton::ControlContentSetDefault(void)noexcept
{
	SetContent(vImageLabel::Create(this));
}
//---------------------------------------------------------------------------
void bcImageTextButton::UpdateZPosition(void)noexcept
{
	bcButton::UpdateZPosition();
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcImageTextButton::SetContent(rPtr<viControl> Content)noexcept
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}
	fContent=cnVar::MoveCast(Content);
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
		fContent->SetView(fView);
	}
}
//---------------------------------------------------------------------------
void bcImageTextButton::ViewSetup(void)noexcept
{
	bcButton::ViewSetup();

	if(fContent!=nullptr){
		fContent->SetView(fView);
	}
	else{
		ControlContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcImageTextButton::ViewClear(void)noexcept
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	bcButton::ViewClear();
}
//---------------------------------------------------------------------------
void bcImageTextButton::Update(void)noexcept
{
	ControlImageTextNotifySet();
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcImageTextButton::UpdateState(void)noexcept
{
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
const uChar16* bcImageTextButton::ControlTextString(void)noexcept
{
	return cnString::TEmptyString<uChar16>::Value;
}
//---------------------------------------------------------------------------
uIntn bcImageTextButton::ControlTextLength(void)noexcept
{
	return 0;
}
//---------------------------------------------------------------------------
const cUITextStyle& bcImageTextButton::ControlTextStyle(void)noexcept
{
	return TextStyle;
}
//---------------------------------------------------------------------------
iBitmapDataSource* bcImageTextButton::ControlImage(void)noexcept
{
	return nullptr;
}
//---------------------------------------------------------------------------
eImageBlendingOperator bcImageTextButton::ControlImageBlendingOperator(void)noexcept
{
	return ImageBlendingOperator::Alpha;
}
//---------------------------------------------------------------------------
cLayoutImageTextOptions bcImageTextButton::ControlImageTextOption(void)noexcept
{
	cLayoutImageTextOptions Options;
	Options.Margin.Left=ContentMarginLeft;
	Options.Margin.Top=ContentMarginTop;
	Options.Margin.Right=ContentMarginRight;
	Options.Margin.Bottom=ContentMarginBottom;
	Options.ImageMargin=ImageMargin;
	Options.TextAlign=TextAlign;
	Options.ImageAlign=ImageAlign;
	return Options;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cImageTextButton::cImageTextButton()noexcept
{
}
//---------------------------------------------------------------------------
cImageTextButton::~cImageTextButton()noexcept
{
}
//---------------------------------------------------------------------------
bool cImageTextButton::ButtonMouseAllowButton(eMouseButton Button)noexcept
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
void cImageTextButton::ButtonClick(const cUIPoint &)noexcept
{
	if(OnClick!=nullptr){
		OnClick();
	}
}
//---------------------------------------------------------------------------
ufInt8 cImageTextButton::CheckState(void)noexcept
{
	return Check;
}
//---------------------------------------------------------------------------
const uChar16* cImageTextButton::ControlTextString(void)noexcept
{
	return Text;
}
//---------------------------------------------------------------------------
uIntn cImageTextButton::ControlTextLength(void)noexcept
{
	return Text->Length;
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImageTextButton::ControlImage(void)noexcept
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImageTextButton::ControlImageBlendingOperator(void)noexcept
{
	return ImageBlendingOperator::Alpha;
}
//---------------------------------------------------------------------------
