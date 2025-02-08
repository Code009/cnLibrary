#include "ImageControl.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnUI::ImageRatioCompare(cUIPoint Size1,cUIPoint Size2)noexcept(true)
{
	return Size1.x*Size2.y < Size1.y*Size2.x;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vImage::vImage(viImageData *Data)noexcept(true)
	: fData(Data)
{
	fUpdateImage=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vImage::~vImage()noexcept(true)
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vImage::Create(viImageData *Data)noexcept(true)
{
	return rCreate< bwvControl<vImage> >(Data);
}
//---------------------------------------------------------------------------
viImageData* vImage::GetData(void)noexcept(true)
{
	return fData;
}
//---------------------------------------------------------------------------
void vImage::SetData(viImageData *Data)noexcept(true)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vImage::Update(void)noexcept(true)
{
	fUpdateImage=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vImage::DataInsertCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fImageDataNotifyToken=fData->ControlImageNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vImage::DataRemoveCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fData->ControlImageNotifySet.Remove(fImageDataNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vImage::Paint(iUISimplePaintContext *Context)noexcept(true)
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
cUIPoint vImage::TransformImageRect()noexcept(true)
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
void vImage::SetupImageCache(void)noexcept(true)
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
cImage::cImage()noexcept(true)
{
}
//---------------------------------------------------------------------------
cImage::~cImage()noexcept(true)
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cImage::ControlContentSetDefault(void)noexcept(true)
{
	SetContent(vImage::Create(this));
}
//---------------------------------------------------------------------------
void cImage::Update(void)noexcept(true)
{
	ControlImageNotifySet();
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImage::ControlImage(void)noexcept(true)
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImage::ControlImageBlendingOperator(void)noexcept(true)
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
eAlignment cImage::ControlImageAlignment(void)noexcept(true)
{
	return ImageAlign;
}
//---------------------------------------------------------------------------
eImageScaleType cImage::ControlImageScale(void)noexcept(true)
{
	return ImageScale;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cButtonImageData::cButtonImageData()noexcept(true)
{
}
//---------------------------------------------------------------------------
cButtonImageData::~cButtonImageData()noexcept(true)
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cButtonImageData::Update(void)noexcept(true)
{
	ButtonImageNotifySet();
}
//---------------------------------------------------------------------------
iBitmapDataSource* cButtonImageData::ButtonImageForState(eButtonState State)noexcept(true)
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
eImageBlendingOperator cButtonImageData::ButtonImageBlendingOperatorForState(eButtonState)noexcept(true)
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
eAlignment cButtonImageData::ButtonImageAlignment(void)noexcept(true)
{
	return ImageAlign;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vButtonImage::vButtonImage(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept(true)
	: fButtonData(ButtonData)
	, fImageData(ImageData)
{
	ButtonDataInsertCallback();
	ImageDataInsertCallback();
}
//---------------------------------------------------------------------------
vButtonImage::~vButtonImage()noexcept(true)
{
	ButtonDataRemoveCallback();
	ImageDataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vButtonImage::Create(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept(true)
{
	return rCreate< bwvControl<vButtonImage> >(ButtonData,ImageData);
}
//---------------------------------------------------------------------------
viButtonData* vButtonImage::GetButtonData(void)const noexcept(true)
{
	return fButtonData;
}
//---------------------------------------------------------------------------
void vButtonImage::SetButtonData(viButtonData *Data)noexcept(true)
{
	if(fButtonData==Data)
		return;

	ButtonDataRemoveCallback();
	fButtonData=Data;
	ButtonDataInsertCallback();

	UpdateState();
}
//---------------------------------------------------------------------------
viButtonImageData* vButtonImage::GetImageData(void)const noexcept(true)
{
	return fImageData;
}
//---------------------------------------------------------------------------
void vButtonImage::SetImageData(viButtonImageData *Data)noexcept(true)
{
	if(fImageData==Data)
		return;

	ImageDataRemoveCallback();
	fImageData=Data;
	ImageDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vButtonImage::SetData(viButtonData *ButtonData,viButtonImageData *ImageData)noexcept(true)
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
void vButtonImage::ButtonDataInsertCallback(void)noexcept(true)
{
	if(fButtonData!=nullptr){
		fButtonDataNofityToken=fButtonData->ButtonNotifySet.Insert([this]{
			UpdateState();
		});
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ButtonDataRemoveCallback(void)noexcept(true)
{
	if(fButtonData!=nullptr){
		fButtonData->ButtonNotifySet.Remove(fButtonDataNofityToken);
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ImageDataInsertCallback(void)noexcept(true)
{
	if(fImageData!=nullptr){
		fButtonDataNofityToken=fImageData->ButtonImageNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ImageDataRemoveCallback(void)noexcept(true)
{
	if(fImageData!=nullptr){
		fImageData->ButtonImageNotifySet.Remove(fButtonDataNofityToken);
	}
}
//---------------------------------------------------------------------------
void vButtonImage::Paint(iUISimplePaintContext *Context)noexcept(true)
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
void vButtonImage::SetupCacheImage(iPtr<iUIBitmap> &CacheImage,eButtonState State)noexcept(true)
{
	auto ImageSource=fImageData->ButtonImageForState(State);
	if(ImageSource==nullptr){
		return;
	}

	auto Device=fViewContent->QueryDevice();
	CacheImage=Device->CreateBitmapCopyFromSource(ImageSource);
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheNormal(void)noexcept(true)
{
	SetupCacheImage(fCacheImageNormal,ButtonState::Normal);
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheHot(void)noexcept(true)
{
	SetupCacheImage(fCacheImageHot,ButtonState::Hot);
	if(fCacheImageHot==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageHot=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCachePressed(void)noexcept(true)
{
	SetupCacheImage(fCacheImagePressed,ButtonState::Pressed);
	if(fCacheImagePressed==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImagePressed=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheInactive(void)noexcept(true)
{
	SetupCacheImage(fCacheImageInactive,ButtonState::Inactive);
	if(fCacheImageInactive==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageInactive=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheDisabled(void)noexcept(true)
{
	SetupCacheImage(fCacheImageDisabled,ButtonState::Disabled);
	if(fCacheImageDisabled==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageDisabled=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ClearCache(void)noexcept(true)
{
	fCacheImageNormal=nullptr;
	fCacheImageHot=nullptr;
	fCacheImagePressed=nullptr;
	fCacheImageDisabled=nullptr;
	fCacheImageInactive=nullptr;
}
//---------------------------------------------------------------------------
void vButtonImage::Update(void)noexcept(true)
{
	ClearCache();
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vButtonImage::UpdateState(void)noexcept(true)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnUI::LayoutImageText(cUIPoint &TextPos,cUIPoint &ImagePos,const cUIPoint &TextSize,const cUIPoint &ImageSize,const cUIPoint &AreaSize,const cLayoutImageTextOptions &Option)noexcept(true)
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
vImageLabel::vImageLabel(viImageTextControlData *Data)noexcept(true)
	: fData(Data)
{
	fUpdateImage=true;
	fUpdateText=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vImageLabel::~vImageLabel()noexcept(true)
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vImageLabel::Create(viImageTextControlData *Data)noexcept(true)
{
	return rCreate< bwvControl<vImageLabel> >(Data);
}
//---------------------------------------------------------------------------
viImageTextControlData* vImageLabel::GetData(void)const noexcept(true)
{
	return fData;
}
//---------------------------------------------------------------------------
void vImageLabel::SetData(viImageTextControlData *Data)noexcept(true)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vImageLabel::DataInsertCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fImageTextNoftifyToken=fData->ControlImageTextNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vImageLabel::DataRemoveCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fData->ControlImageTextNotifySet.Remove(fImageTextNoftifyToken);
	}
}
//---------------------------------------------------------------------------
void vImageLabel::Update(void)noexcept(true)
{
	fUpdateImage=true;
	fUpdateText=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vImageLabel::Paint(iUISimplePaintContext *Context)noexcept(true)
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
void vImageLabel::SetupTextCache(void)noexcept(true)
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
void vImageLabel::SetupImageCache(void)noexcept(true)
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
cImageLabel::cImageLabel()noexcept(true)
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
cImageLabel::~cImageLabel()noexcept(true)
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cImageLabel::ControlContentSetDefault(void)noexcept(true)
{
	SetContent(vImageLabel::Create(this));
}
//---------------------------------------------------------------------------
void cImageLabel::Update(void)noexcept(true)
{
	ControlImageTextNotifySet();
}
//---------------------------------------------------------------------------
const uChar16* cImageLabel::ControlTextString(void)noexcept(true)
{
	return Text->Pointer;
}
//---------------------------------------------------------------------------
uIntn cImageLabel::ControlTextLength(void)noexcept(true)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
const cUITextStyle& cImageLabel::ControlTextStyle(void)noexcept(true)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImageLabel::ControlImage(void)noexcept(true)
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImageLabel::ControlImageBlendingOperator(void)noexcept(true)
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
cLayoutImageTextOptions cImageLabel::ControlImageTextOption(void)noexcept(true)
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
bcImageTextButton::bcImageTextButton()noexcept(true)
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
bcImageTextButton::~bcImageTextButton()noexcept(true)
{
	SetView(nullptr);
	viImageTextControlData::InvalidateData();
}
//---------------------------------------------------------------------------
void bcImageTextButton::ControlContentSetDefault(void)noexcept(true)
{
	SetContent(vImageLabel::Create(this));
}
//---------------------------------------------------------------------------
void bcImageTextButton::UpdateZPosition(void)noexcept(true)
{
	bcButton::UpdateZPosition();
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcImageTextButton::SetContent(rPtr<viControl> Content)noexcept(true)
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
void bcImageTextButton::ViewSetup(void)noexcept(true)
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
void bcImageTextButton::ViewClear(void)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	bcButton::ViewClear();
}
//---------------------------------------------------------------------------
void bcImageTextButton::Update(void)noexcept(true)
{
	ControlImageTextNotifySet();
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcImageTextButton::UpdateState(void)noexcept(true)
{
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
const uChar16* bcImageTextButton::ControlTextString(void)noexcept(true)
{
	return cnString::TEmptyString<uChar16>::Value;
}
//---------------------------------------------------------------------------
uIntn bcImageTextButton::ControlTextLength(void)noexcept(true)
{
	return 0;
}
//---------------------------------------------------------------------------
const cUITextStyle& bcImageTextButton::ControlTextStyle(void)noexcept(true)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
iBitmapDataSource* bcImageTextButton::ControlImage(void)noexcept(true)
{
	return nullptr;
}
//---------------------------------------------------------------------------
eImageBlendingOperator bcImageTextButton::ControlImageBlendingOperator(void)noexcept(true)
{
	return ImageBlendingOperator::Alpha;
}
//---------------------------------------------------------------------------
cLayoutImageTextOptions bcImageTextButton::ControlImageTextOption(void)noexcept(true)
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
cImageTextButton::cImageTextButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
cImageTextButton::~cImageTextButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cImageTextButton::ButtonMouseAllowButton(eMouseButton Button)noexcept(true)
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
void cImageTextButton::ButtonClick(const cUIPoint &)noexcept(true)
{
	if(OnClick!=nullptr){
		OnClick();
	}
}
//---------------------------------------------------------------------------
ufInt8 cImageTextButton::CheckState(void)noexcept(true)
{
	return Check;
}
//---------------------------------------------------------------------------
const uChar16* cImageTextButton::ControlTextString(void)noexcept(true)
{
	return Text;
}
//---------------------------------------------------------------------------
uIntn cImageTextButton::ControlTextLength(void)noexcept(true)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImageTextButton::ControlImage(void)noexcept(true)
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImageTextButton::ControlImageBlendingOperator(void)noexcept(true)
{
	return ImageBlendingOperator::Alpha;
}
//---------------------------------------------------------------------------
