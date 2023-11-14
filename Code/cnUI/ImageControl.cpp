#include "ImageControl.h"

using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool cnUI::ImageRatioCompare(cUIPoint Size1,cUIPoint Size2)
{
	return Size1.x*Size2.y < Size1.y*Size2.x;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vImage::vImage(viImageData *Data)
	: fData(Data)
{
	fUpdateImage=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vImage::~vImage()
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vImage::Create(viImageData *Data)
{
	return rCreate< bwvControl<vImage> >(Data);
}
//---------------------------------------------------------------------------
viImageData* vImage::GetData(void)
{
	return fData;
}
//---------------------------------------------------------------------------
void vImage::SetData(viImageData *Data)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vImage::Update(void)
{
	fUpdateImage=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vImage::DataInsertCallback(void)
{
	if(fData!=nullptr){
		fImageDataNotifyToken=fData->ControlImageNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vImage::DataRemoveCallback(void)
{
	if(fData!=nullptr){
		fData->ControlImageNotifySet.Remove(fImageDataNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vImage::Paint(iUISimplePaintContext *Context)
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
cUIPoint vImage::TransformImageRect()
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
void vImage::SetupImageCache(void)
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
cImage::cImage()
{
}
//---------------------------------------------------------------------------
cImage::~cImage()
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cImage::ControlContentSetDefault(void)
{
	SetContent(vImage::Create(this));
}
//---------------------------------------------------------------------------
void cImage::Update(void)
{
	ControlImageNotifySet();
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImage::ControlImage(void)
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImage::ControlImageBlendingOperator(void)
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
eAlignment cImage::ControlImageAlignment(void)
{
	return ImageAlign;
}
//---------------------------------------------------------------------------
eImageScaleType cImage::ControlImageScale(void)
{
	return ImageScale;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cButtonImageData::cButtonImageData()
{
}
//---------------------------------------------------------------------------
cButtonImageData::~cButtonImageData()
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cButtonImageData::Update(void)
{
	ButtonImageNotifySet();
}
//---------------------------------------------------------------------------
iBitmapDataSource* cButtonImageData::ButtonImageForState(eButtonState State)
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
eImageBlendingOperator cButtonImageData::ButtonImageBlendingOperatorForState(eButtonState)
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
eAlignment cButtonImageData::ButtonImageAlignment(void)
{
	return ImageAlign;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vButtonImage::vButtonImage(viButtonData *ButtonData,viButtonImageData *ImageData)
	: fButtonData(ButtonData)
	, fImageData(ImageData)
{
	ButtonDataInsertCallback();
	ImageDataInsertCallback();
}
//---------------------------------------------------------------------------
vButtonImage::~vButtonImage()
{
	ButtonDataRemoveCallback();
	ImageDataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vButtonImage::Create(viButtonData *ButtonData,viButtonImageData *ImageData)
{
	return rCreate< bwvControl<vButtonImage> >(ButtonData,ImageData);
}
//---------------------------------------------------------------------------
viButtonData* vButtonImage::GetButtonData(void)const
{
	return fButtonData;
}
//---------------------------------------------------------------------------
void vButtonImage::SetButtonData(viButtonData *Data)
{
	if(fButtonData==Data)
		return;

	ButtonDataRemoveCallback();
	fButtonData=Data;
	ButtonDataInsertCallback();

	UpdateState();
}
//---------------------------------------------------------------------------
viButtonImageData* vButtonImage::GetImageData(void)const
{
	return fImageData;
}
//---------------------------------------------------------------------------
void vButtonImage::SetImageData(viButtonImageData *Data)
{
	if(fImageData==Data)
		return;

	ImageDataRemoveCallback();
	fImageData=Data;
	ImageDataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vButtonImage::SetData(viButtonData *ButtonData,viButtonImageData *ImageData)
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
void vButtonImage::ButtonDataInsertCallback(void)
{
	if(fButtonData!=nullptr){
		fButtonDataNofityToken=fButtonData->ButtonNotifySet.Insert([this]{
			UpdateState();
		});
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ButtonDataRemoveCallback(void)
{
	if(fButtonData!=nullptr){
		fButtonData->ButtonNotifySet.Remove(fButtonDataNofityToken);
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ImageDataInsertCallback(void)
{
	if(fImageData!=nullptr){
		fButtonDataNofityToken=fImageData->ButtonImageNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ImageDataRemoveCallback(void)
{
	if(fImageData!=nullptr){
		fImageData->ButtonImageNotifySet.Remove(fButtonDataNofityToken);
	}
}
//---------------------------------------------------------------------------
void vButtonImage::Paint(iUISimplePaintContext *Context)
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
void vButtonImage::SetupCacheImage(iPtr<iUIBitmap> &CacheImage,eButtonState State)
{
	auto ImageSource=fImageData->ButtonImageForState(State);
	if(ImageSource==nullptr){
		return;
	}

	auto Device=fViewContent->QueryDevice();
	CacheImage=Device->CreateBitmapCopyFromSource(ImageSource);
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheNormal(void)
{
	SetupCacheImage(fCacheImageNormal,ButtonState::Normal);
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheHot(void)
{
	SetupCacheImage(fCacheImageHot,ButtonState::Hot);
	if(fCacheImageHot==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageHot=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCachePressed(void)
{
	SetupCacheImage(fCacheImagePressed,ButtonState::Pressed);
	if(fCacheImagePressed==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImagePressed=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheInactive(void)
{
	SetupCacheImage(fCacheImageInactive,ButtonState::Inactive);
	if(fCacheImageInactive==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageInactive=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::SetupCacheDisabled(void)
{
	SetupCacheImage(fCacheImageDisabled,ButtonState::Disabled);
	if(fCacheImageDisabled==nullptr){
		// process hot image from normal
		SetupCacheNormal();

		fCacheImageDisabled=fCacheImageNormal;
	}
}
//---------------------------------------------------------------------------
void vButtonImage::ClearCache(void)
{
	fCacheImageNormal=nullptr;
	fCacheImageHot=nullptr;
	fCacheImagePressed=nullptr;
	fCacheImageDisabled=nullptr;
	fCacheImageInactive=nullptr;
}
//---------------------------------------------------------------------------
void vButtonImage::Update(void)
{
	ClearCache();
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vButtonImage::UpdateState(void)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cnUI::LayoutImageText(cUIPoint &TextPos,cUIPoint &ImagePos,const cUIPoint &TextSize,const cUIPoint &ImageSize,const cUIPoint &AreaSize,const cLayoutImageTextOptions &Option)
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
vImageLabel::vImageLabel(viImageTextControlData *Data)
	: fData(Data)
{
	fUpdateImage=true;
	fUpdateText=true;
	DataInsertCallback();
}
//---------------------------------------------------------------------------
vImageLabel::~vImageLabel()
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vImageLabel::Create(viImageTextControlData *Data)
{
	return rCreate< bwvControl<vImageLabel> >(Data);
}
//---------------------------------------------------------------------------
viImageTextControlData* vImageLabel::GetData(void)const
{
	return fData;
}
//---------------------------------------------------------------------------
void vImageLabel::SetData(viImageTextControlData *Data)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vImageLabel::DataInsertCallback(void)
{
	if(fData!=nullptr){
		fImageTextNoftifyToken=fData->ControlImageTextNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vImageLabel::DataRemoveCallback(void)
{
	if(fData!=nullptr){
		fData->ControlImageTextNotifySet.Remove(fImageTextNoftifyToken);
	}
}
//---------------------------------------------------------------------------
void vImageLabel::Update(void)
{
	fUpdateImage=true;
	fUpdateText=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vImageLabel::Paint(iUISimplePaintContext *Context)
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
void vImageLabel::SetupTextCache(void)
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
void vImageLabel::SetupImageCache(void)
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
cImageLabel::cImageLabel()
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
cImageLabel::~cImageLabel()
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void cImageLabel::ControlContentSetDefault(void)
{
	SetContent(vImageLabel::Create(this));
}
//---------------------------------------------------------------------------
void cImageLabel::Update(void)
{
	ControlImageTextNotifySet();
}
//---------------------------------------------------------------------------
const uChar16* cImageLabel::ControlTextString(void)
{
	return Text->Pointer;
}
//---------------------------------------------------------------------------
uIntn cImageLabel::ControlTextLength(void)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
const cUITextStyle& cImageLabel::ControlTextStyle(void)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImageLabel::ControlImage(void)
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImageLabel::ControlImageBlendingOperator(void)
{
	return ImageBlend;
}
//---------------------------------------------------------------------------
cLayoutImageTextOptions cImageLabel::ControlImageTextOption(void)
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
bcImageTextButton::bcImageTextButton()
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
bcImageTextButton::~bcImageTextButton()
{
	SetView(nullptr);
	viImageTextControlData::InvalidateData();
}
//---------------------------------------------------------------------------
void bcImageTextButton::ControlContentSetDefault(void)
{
	SetContent(vImageLabel::Create(this));
}
//---------------------------------------------------------------------------
void bcImageTextButton::UpdateZPosition(void)
{
	bcButton::UpdateZPosition();
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcImageTextButton::SetContent(rPtr<viControl> Content)
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
void bcImageTextButton::ViewSetup(void)
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
void bcImageTextButton::ViewClear(void)
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	bcButton::ViewClear();
}
//---------------------------------------------------------------------------
void bcImageTextButton::Update(void)
{
	ControlImageTextNotifySet();
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcImageTextButton::UpdateState(void)
{
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
const uChar16* bcImageTextButton::ControlTextString(void)
{
	return cnString::TEmptyString<uChar16>::Value;
}
//---------------------------------------------------------------------------
uIntn bcImageTextButton::ControlTextLength(void)
{
	return 0;
}
//---------------------------------------------------------------------------
const cUITextStyle& bcImageTextButton::ControlTextStyle(void)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
iBitmapDataSource* bcImageTextButton::ControlImage(void)
{
	return nullptr;
}
//---------------------------------------------------------------------------
eImageBlendingOperator bcImageTextButton::ControlImageBlendingOperator(void)
{
	return ImageBlendingOperator::Alpha;
}
//---------------------------------------------------------------------------
cLayoutImageTextOptions bcImageTextButton::ControlImageTextOption(void)
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
cImageTextButton::cImageTextButton()
{
}
//---------------------------------------------------------------------------
cImageTextButton::~cImageTextButton()
{
}
//---------------------------------------------------------------------------
bool cImageTextButton::ButtonMouseAllowButton(eMouseButton Button)
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
void cImageTextButton::ButtonClick(const cUIPoint &)
{
	if(OnClick!=nullptr){
		OnClick();
	}
}
//---------------------------------------------------------------------------
ufInt8 cImageTextButton::CheckState(void)
{
	return Check;
}
//---------------------------------------------------------------------------
const uChar16* cImageTextButton::ControlTextString(void)
{
	return Text;
}
//---------------------------------------------------------------------------
uIntn cImageTextButton::ControlTextLength(void)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
iBitmapDataSource* cImageTextButton::ControlImage(void)
{
	return Image;
}
//---------------------------------------------------------------------------
eImageBlendingOperator cImageTextButton::ControlImageBlendingOperator(void)
{
	return ImageBlendingOperator::Alpha;
}
//---------------------------------------------------------------------------
