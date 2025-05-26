#include "BasicControl.h"
#include "Shape.h"

using namespace cnLibrary;
using namespace cnUI;

rPtr<viControl>				(*cnUI::gCreateDefaultButtonVisual)(viButtonData *Data)noexcept(true)		=vButton::Create;
rPtr<viScrollBar>			(*cnUI::gCreateDefaultScrollBarVisual)(viScrollBarData *Data)noexcept(true)	=nullptr;

static rPtr<viControl> CreateDefaultSplitterBarVisual(void)noexcept(true){
	return vSolidStaticColor::Create(UIColorFromUInt32(0xFF0000FF));
}
rPtr<viControl>				(*cnUI::gCreateDefaultSplitterBarVisual)(void)noexcept(true)					=CreateDefaultSplitterBarVisual;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lFrame::lFrame()noexcept(true)
	: ClientMargin{0,0,0,0}
{
	
}
//---------------------------------------------------------------------------
lFrame::~lFrame()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
iUIView* lFrame::GetClientView(void)noexcept(true)
{
	return fClientView;
}
//---------------------------------------------------------------------------
void lFrame::SetClientView(iUIView *View)noexcept(true)
{
	if(fView!=nullptr){
		if(fClientView!=nullptr){
			fView->RemoveView(fClientView);
		}
		fClientView=View;
		if(fClientView!=nullptr){
			fView->InsertView(fClientView);
		}
	}
	else{
		fClientView=View;
	}
}
//---------------------------------------------------------------------------
void lFrame::ViewSetup(void)noexcept(true)
{
	VisualControl::ViewSetup();

	if(fClientView!=nullptr){
		fView->InsertView(fClientView);
	}
}
//---------------------------------------------------------------------------
void lFrame::ViewClear(void)noexcept(true)
{
	if(fClientView!=nullptr){
		fView->RemoveView(fClientView);
	}

	VisualControl::ViewClear();
}
//---------------------------------------------------------------------------
void lFrame::UILayout(void)noexcept(true)
{
	VisualControl::UILayout();

	if(fClientView!=nullptr){
		cLayout Layout;
		Layout.ResetClient(fView);


		ViewSetRect(fClientView,fView,Layout.Client(ClientMargin));
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cContent::cContent()noexcept(true)
{
}
//---------------------------------------------------------------------------
cContent::~cContent()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
void cContent::SetContent(rPtr<viControl> Content)noexcept(true)
{
	if(fContent==Content)
		return;

	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}
	fContent=cnVar::MoveCast(Content);
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
		fContent->SetView(fView);
	}
	ControlContentChanged();
}
//---------------------------------------------------------------------------
void cContent::UpdateContentZPosition(void)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void cContent::ViewSetup(void)noexcept(true)
{
	Control::ViewSetup();
	
	if(fContent!=nullptr){
		fContent->SetView(fView);
	}
	else{
		ControlContentSetDefault();
	}

	
}
//---------------------------------------------------------------------------
void cContent::ViewClear(void)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	Control::ViewClear();
}
//---------------------------------------------------------------------------
void cContent::ControlContentSetDefault(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cContent::ControlContentChanged(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cVisual::cVisual()noexcept(true)
{
}
//---------------------------------------------------------------------------
cVisual::~cVisual()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
Float32 cVisual::ControlContentZPosition(void)noexcept(true)
{
	return ZPosition_Content+ControlZOffset;
}
//---------------------------------------------------------------------------
void cVisual::UpdateControlZOffset(void)noexcept(true)
{
	VisualControl::UpdateControlZOffset();

	if(fContent!=nullptr){
		auto ZPosition=ControlContentZPosition();
		fContent->SetContentZPosition(ZPosition);
	}
}
//---------------------------------------------------------------------------
void cVisual::ControlContentSetDefault(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void cVisual::SetContent(rPtr<viControl> Content)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}
	fContent=cnVar::MoveCast(Content);
	if(fContent!=nullptr){
		Float32 ZPosition=ControlContentZPosition();
		fContent->SetContentZPosition(ZPosition);
		fContent->SetView(fView);
	}
}
//---------------------------------------------------------------------------
void cVisual::ViewSetup(void)noexcept(true)
{
	VisualControl::ViewSetup();

	if(fContent!=nullptr){
		fContent->SetView(fView);
	}
	else{
		ControlContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void cVisual::ViewClear(void)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	VisualControl::ViewClear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vLabel::vLabel(viTextControlData *Data)noexcept(true)
	: fData(Data)
{
	fUpdateText=true;

	DataInsertCallback();
}
//---------------------------------------------------------------------------
vLabel::~vLabel()noexcept(true)
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vLabel::Create(viTextControlData *Data)noexcept(true)
{
	return rCreate< bwvControl<vLabel> >(Data);
}
//---------------------------------------------------------------------------
viTextControlData* vLabel::GetData(void)const noexcept(true)
{
	return fData;
}
//---------------------------------------------------------------------------
void vLabel::SetData(viTextControlData *Data)noexcept(true)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
void vLabel::Update(void)noexcept(true)
{
	fUpdateText=true;
	if(fViewContent!=nullptr)
		fViewContent->QueryUpdate();
}
//---------------------------------------------------------------------------
void vLabel::DataInsertCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fTextControlNotifyToken=fData->ControlTextNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vLabel::DataRemoveCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fData->ControlTextNotifySet.Remove(fTextControlNotifyToken);
	}
}
//---------------------------------------------------------------------------
void vLabel::Paint(iUISimplePaintContext *Context)noexcept(true)
{
	SetupTextCache();

	if(fCacheTextGraph==nullptr)
		return;

	auto TextAlign=fData->ControlTextAlignment();
	auto TextMargin=fData->ControlTextMargin();

	// text area
	cUIPoint TextAreaPos;
	cUIPoint TextAreaSize=fPaintSize;

	TextAreaPos.x=TextMargin.Left;
	TextAreaPos.y=TextMargin.Top;
	
	TextAreaSize.x-=TextMargin.Left+TextMargin.Right;
	TextAreaSize.y-=TextMargin.Top+TextMargin.Bottom;

	// text

	cUIPoint TextPos=RectAlignInRect(TextAlign,TextAreaSize,fTextGraphSize);
	TextPos.x+=TextAreaPos.x;
	TextPos.y+=TextAreaPos.y;

	// draw text
	Context->Graph(TextPos,fTextGraphSize,fCacheTextGraph);
}
//---------------------------------------------------------------------------
void vLabel::SetupTextCache(void)noexcept(true)
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

	auto ContentScale=fViewContent->GetContentScale();
	ContentScale*Style.FontSize;
	Style.FontSize;
	fCacheTextGraph=Device->CreateTextGraph(String,Length,Style,nullptr);
	if(fCacheTextGraph==nullptr)
		return;

	fTextGraphSize=fCacheTextGraph->GetSize();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cLabel::cLabel()noexcept(true)
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
cLabel::~cLabel()noexcept(true)
{
	SetContent(nullptr);
	InvalidateData();
}
//---------------------------------------------------------------------------
void cLabel::ControlContentSetDefault(void)noexcept(true)
{
	SetContent(vLabel::Create(this));
}
//---------------------------------------------------------------------------
void cLabel::Update(void)noexcept(true)
{
	ControlTextNotifySet();
}
//---------------------------------------------------------------------------
const uChar16* cLabel::ControlTextString(void)noexcept(true)
{
	return Text->Pointer;
}
//---------------------------------------------------------------------------
uIntn cLabel::ControlTextLength(void)noexcept(true)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
const cUITextStyle& cLabel::ControlTextStyle(void)noexcept(true)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
cUIRectangle cLabel::ControlTextMargin(void)noexcept(true)
{
	if(fView==nullptr)
		return UIRectangleZero;
	return fView->GetFrameMargin();
}
//---------------------------------------------------------------------------
eAlignment cLabel::ControlTextAlignment(void)noexcept(true)
{
	return TextAlign;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
vButton::vButton(viButtonData *Data)noexcept(true)
	: fData(Data)
{
	DataInsertCallback();
	LayoutOrder=LayoutOrder_Background;
}
//---------------------------------------------------------------------------
vButton::~vButton()noexcept(true)
{
	DataRemoveCallback();
}
//---------------------------------------------------------------------------
rPtr<viControl> vButton::Create(viButtonData *Data)noexcept(true)
{
	return rCreate< bwvControl<vButton> >(Data);
}
//---------------------------------------------------------------------------
void vButton::SetView(iUIView *View)noexcept(true)
{
	ViewControl::SetView(View);
	vSimpleViewPainter::SetView(View);
}
//---------------------------------------------------------------------------
void vButton::SetContentZPosition(Float32 ZPosition)noexcept(true)
{
	vSimpleViewPainter::SetContentZPosition(ZPosition);
}
//---------------------------------------------------------------------------
cUIRectangle vButton::UIMargin(const cUIRectangle &Margin)noexcept(true)
{
	cUIRectangle FrameMargin={2,2,2,2};

	return MergeMargin(FrameMargin,Margin);
}
//---------------------------------------------------------------------------
viButtonData* vButton::GetData(void)const noexcept(true)
{
	return fData;
}
//---------------------------------------------------------------------------
void vButton::SetData(viButtonData *Data)noexcept(true)
{
	if(fData==Data)
		return;

	DataRemoveCallback();
	fData=Data;
	DataInsertCallback();

	Update();
}
//---------------------------------------------------------------------------
const cUIColor vButton::StateColorNormal=UIColorFromUInt32(0xFFCCCCCC);
const cUIColor vButton::StateColorHot=UIColorFromUInt32(0xFFFF0000);
const cUIColor vButton::StateColorPressed=UIColorFromUInt32(0xFF0000FF);
const cUIColor vButton::StateColorDisabled=UIColorFromUInt32(0xFF000000);
//---------------------------------------------------------------------------
void vButton::Paint(iUISimplePaintContext *Context)noexcept(true)
{
	if(fData==nullptr)
		return;

	auto BtnState=fData->ButtonState();
	// draw button image
	switch(BtnState){
	default:
	case ButtonState::Inactive:
	case ButtonState::Normal:
		Context->Fill({0,0},fPaintSize,StateColorNormal);
		break;
	case ButtonState::Hot:
		Context->Fill({0,0},fPaintSize,StateColorHot);
		break;
	case ButtonState::Pressed:
		Context->Fill({0,0},fPaintSize,StateColorPressed);
		break;
	case ButtonState::Disabled:
		Context->Fill({0,0},fPaintSize,StateColorDisabled);
		break;
	}
}
//---------------------------------------------------------------------------
void vButton::Update(void)noexcept(true)
{
	if(fViewContent!=nullptr){
		fViewContent->QueryUpdate();
	}
}
//---------------------------------------------------------------------------
void vButton::DataInsertCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fButtonNotifyToken=fData->ButtonNotifySet.Insert([this]{
			Update();
		});
	}
}
//---------------------------------------------------------------------------
void vButton::DataRemoveCallback(void)noexcept(true)
{
	if(fData!=nullptr){
		fData->ButtonNotifySet.Remove(fButtonNotifyToken);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcButton::bcButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcButton::~bcButton()noexcept(true)
{
	InvalidateData();
}
//---------------------------------------------------------------------------
void bcButton::ViewSetup(void)noexcept(true)
{
	Control::ViewSetup();
	fHot=false;
	fMouseDown=false;
	fTouchDown=false;
	fMouseDownBtn=MouseButton::None;
	fTouchDownPointID=nullptr;

	if(fBackground!=nullptr){
		fBackground->SetView(fView);
	}
	else{
		ButtonBackgroundSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcButton::ViewClear(void)noexcept(true)
{
	fMouseDown=false;
	fTouchDown=false;
	fHot=false;

	fMouseDownBtn=MouseButton::None;
	fTouchDownPointID=nullptr;

	if(fBackground!=nullptr){
		fBackground->SetView(nullptr);
	}

	Control::ViewClear();
}
//---------------------------------------------------------------------------
void bcButton::SetBackground(rPtr<viControl> Background)noexcept(true)
{
	if(fBackground!=nullptr){
		fBackground->SetView(nullptr);
	}
	fBackground=cnVar::MoveCast(Background);
	if(fBackground!=nullptr){
		fBackground->SetContentZPosition(BackgroundZPosition);
		fBackground->SetView(fView);

		ButtonBackgroundChanged();
	}
}
//---------------------------------------------------------------------------
void bcButton::UpdateZPosition(void)noexcept(true)
{
	if(fBackground!=nullptr){
		fBackground->SetContentZPosition(BackgroundZPosition);
	}
}
//---------------------------------------------------------------------------
void bcButton::ButtonBackgroundSetDefault(void)noexcept(true)
{
	if(gCreateDefaultButtonVisual!=nullptr){
		SetBackground(gCreateDefaultButtonVisual(this));
	}
}
//---------------------------------------------------------------------------
void bcButton::ButtonBackgroundChanged(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcButton::MouseEnter(iUIMouseEvent*)noexcept(true)
{
	fHot=true;
	fMouseDownBtn=MouseButton::None;
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcButton::MouseLeave(iUIMouseEvent*)noexcept(true)
{
	fHot=false;
	fMouseDown=false;
	fMouseDownBtn=MouseButton::None;

	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcButton::MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)
{
	bool PrevDown=fMouseDown;
	if(fMouseDownBtn!=MouseButton::None){
		cUIPoint Pos;
		MouseEvent->GetPosition(fView,Pos);
		bool IsInRange=ViewIsPointInside(fView,Pos.x,Pos.y);
		if(IsInRange){
			fMouseDown=true;
		}
		else{
			fMouseDown=false;
		}
	}
	else{
		fMouseDown=false;
	}
	if(fMouseDown!=PrevDown){
		ButtonStateChanged();
	}
}
//---------------------------------------------------------------------------
void bcButton::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{	UnusedParameter(MouseEvent);
	if(fMouseDownBtn!=MouseButton::None)
		return;

	if(ButtonMouseAllowButton(Button)==false){
		return;
	}
	fMouseDownBtn=Button;

	fMouseDown=true;

	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcButton::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	if(Button==fMouseDownBtn){
		fMouseDown=false;

		fMouseDownBtn=MouseButton::None;

		ButtonStateChanged();
		cUIPoint Pos;
		if(MouseEvent->GetPosition(fView,Pos)){
			if(ViewIsPointInside(fView,Pos.x,Pos.y)){
				ButtonClick(Pos);
			}
		}
	}

}
//---------------------------------------------------------------------------
void bcButton::TouchDown(iUITouchEvent *TouchEvent)noexcept(true)
{
	if(fTouchDownPointID!=nullptr)
		return;

	fTouchDownPointID=TouchEvent->GetTouchID();

	fTouchDown=true;

	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void bcButton::TouchUp(iUITouchEvent *TouchEvent)noexcept(true)
{
	if(TouchEvent->GetTouchID()==fTouchDownPointID){
		fTouchDown=false;

		fTouchDownPointID=nullptr;

		ButtonStateChanged();
		cUIPoint Pos;
		if(TouchEvent->GetPosition(fView,Pos)){
			if(ViewIsPointInside(fView,Pos.x,Pos.y)){
				ButtonClick(Pos);
			}
		}
	}
}
//---------------------------------------------------------------------------
void bcButton::TouchLost(iUITouchEvent *TouchEvent)noexcept(true)
{
	if(TouchEvent->GetTouchID()==fTouchDownPointID){
		fTouchDown=false;

		fTouchDownPointID=nullptr;
	}
}
//---------------------------------------------------------------------------
void bcButton::TouchMove(iUITouchEvent *TouchEvent)noexcept(true)
{
	if(fTouchDownPointID!=TouchEvent->GetTouchID()){
		return;
	}
	bool PrevDown=fTouchDown;
	cUIPoint Pos;
	TouchEvent->GetPosition(fView,Pos);
	bool IsInRange=ViewIsPointInside(fView,Pos.x,Pos.y);
	fTouchDown=IsInRange;

	if(fTouchDown!=PrevDown){
		ButtonStateChanged();
	}
}
//---------------------------------------------------------------------------
eButtonState bcButton::ButtonState(void)noexcept(true)
{
	return GetButtonState();
}
//---------------------------------------------------------------------------
ufInt8 bcButton::CheckState(void)noexcept(true)
{
	return 0;
}
//---------------------------------------------------------------------------
void bcButton::ButtonStateChanged(void)noexcept(true)
{
	ButtonNotifySet();
}
//---------------------------------------------------------------------------
eButtonState bcButton::GetButtonState(void)const noexcept(true)
{
	if(fView->IsEnabled()==false){
		return ButtonState::Disabled;
	}
	if(fTouchDown || fMouseDown){
		return ButtonState::Pressed;
	}
	if(fHot){
		return ButtonState::Hot;
	}
	return ButtonState::Normal;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cButton::cButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
cButton::~cButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cButton::ButtonClick(const cUIPoint &)noexcept(true)
{
	if(OnClick!=nullptr){
		OnClick();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcTextButton::bcTextButton()noexcept(true)
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
bcTextButton::~bcTextButton()noexcept(true)
{
	SetView(nullptr);
	viButtonData::InvalidateData();
	viTextControlData::InvalidateData();
}
//---------------------------------------------------------------------------
void bcTextButton::ViewSetup(void)noexcept(true)
{
	bcButton::ViewSetup();

	if(fContent!=nullptr){
		fContent->SetView(fView);
	}
	else{
		ButtonContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcTextButton::ViewClear(void)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	bcButton::ViewClear();
}
//---------------------------------------------------------------------------
void bcTextButton::UpdateZPosition(void)noexcept(true)
{
	bcButton::UpdateZPosition();

	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcTextButton::ButtonContentSetDefault(void)noexcept(true)
{
	SetContent(vLabel::Create(this));
}
//---------------------------------------------------------------------------
void bcTextButton::SetContent(rPtr<viControl> Content)noexcept(true)
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
const uChar16* bcTextButton::ControlTextString(void)noexcept(true)
{
	return cnString::TEmptyString<uChar16>::Value;
}
//---------------------------------------------------------------------------
uIntn bcTextButton::ControlTextLength(void)noexcept(true)
{
	return 0;
}
//---------------------------------------------------------------------------
const cUITextStyle& bcTextButton::ControlTextStyle(void)noexcept(true)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
cUIRectangle bcTextButton::ControlTextMargin(void)noexcept(true)
{
	if(fView==nullptr)
		return UIRectangleZero;
	auto Margin=fView->GetFrameMargin();
	Margin.Left+=ContentMarginLeft;
	Margin.Top+=ContentMarginTop;
	Margin.Right+=ContentMarginRight;
	Margin.Bottom+=ContentMarginBottom;
	return Margin;
}
//---------------------------------------------------------------------------
eAlignment bcTextButton::ControlTextAlignment(void)noexcept(true)
{
	return TextAlign;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTextButton::cTextButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
cTextButton::~cTextButton()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool cTextButton::ButtonMouseAllowButton(eMouseButton Button)noexcept(true)
{
	return Button==MouseButton::Left;
}
//---------------------------------------------------------------------------
void cTextButton::ButtonClick(const cUIPoint &)noexcept(true)
{
	if(OnClick!=nullptr){
		OnClick();
	}
}
//---------------------------------------------------------------------------
ufInt8 cTextButton::CheckState(void)noexcept(true)
{
	return Check;
}
//---------------------------------------------------------------------------
const uChar16* cTextButton::ControlTextString(void)noexcept(true)
{
	return Text;
}
//---------------------------------------------------------------------------
uIntn cTextButton::ControlTextLength(void)noexcept(true)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
void cTextButton::Update(void)noexcept(true)
{
	ControlTextNotifySet();
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
void cTextButton::UpdateState(void)noexcept(true)
{
	ButtonStateChanged();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcScrollBar::bcScrollBar()noexcept(true)
{
	fActiveButton=eScrollBarButton::None;

	fButtonDown=false;
	fHot=false;
}
//---------------------------------------------------------------------------
bcScrollBar::~bcScrollBar()noexcept(true)
{
	SetView(nullptr);
	InvalidateData();
}
//---------------------------------------------------------------------------
void bcScrollBar::ViewSetup(void)noexcept(true)
{
	Control::ViewSetup();
	
	if(fContent!=nullptr){
		fContent->SetView(fView);
	}
	else{
		ControlContentSetDefault();
	}

	auto UIDispatch=cnSystem::CurrentUIThread::GetDispatch(false);
	fMousePressTimer=UIDispatch->CreateTimer(nullptr,&fMousePressTimerProc);
}
//---------------------------------------------------------------------------
void bcScrollBar::ViewClear(void)noexcept(true)
{
	fMousePressTimer->Stop();
	fMousePressTimer=nullptr;

	if(fContent!=nullptr){
		fContent->SetView(nullptr);
	}

	Control::ViewClear();
}
//---------------------------------------------------------------------------
void bcScrollBar::ControlContentSetDefault(void)noexcept(true)
{
	if(gCreateDefaultScrollBarVisual!=nullptr){
		SetContent(gCreateDefaultScrollBarVisual(this));
	}
}
//---------------------------------------------------------------------------
void bcScrollBar::UpdateZPosition(void)noexcept(true)
{
	if(fContent!=nullptr){
		fContent->SetContentZPosition(ContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcScrollBar::SetContent(rPtr<viScrollBar> Content)noexcept(true)
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
void bcScrollBar::ScrollBarNotify(void)noexcept(true)
{
	ScrollBarNotifySet();
}
//---------------------------------------------------------------------------
eScrollBarButton bcScrollBar::ScrollBarActiveButton(void)noexcept(true)
{
	return fActiveButton;
}
//---------------------------------------------------------------------------
eButtonState bcScrollBar::ScrollBarButtonState(void)noexcept(true)
{
	if(fButtonDown){
		return ButtonState::Pressed;
	}
	if(fHot)
		return ButtonState::Hot;
	return ButtonState::Normal;
}
//---------------------------------------------------------------------------
viScrollBarData::cScrollPosition bcScrollBar::ScrollBarPosition(void)noexcept(true)
{
	cScrollPosition ScrollPos;
	ScrollPos.View.Begin=0;
	ScrollPos.View.End=0;
	ScrollPos.Total.Begin=0;
	ScrollPos.Total.End=0;
	return ScrollPos;
}
//---------------------------------------------------------------------------
void bcScrollBar::ScrollBarMove(Float32)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcScrollBar::ScrollBarMoveAction(bool,eScrollBarMoveDistance)noexcept(true)
{
}
//---------------------------------------------------------------------------
void bcScrollBar::MouseEnter(iUIMouseEvent*)noexcept(true)
{
	fActiveButton=eScrollBarButton::None;
	fHot=true;
}
//---------------------------------------------------------------------------
void bcScrollBar::MouseLeave(iUIMouseEvent*)noexcept(true)
{
	fActiveButton=eScrollBarButton::None;
	fHot=false;
	fButtonDown=false;
	
	fMousePressTimer->Stop();

	ScrollBarNotify();
}
//---------------------------------------------------------------------------
void bcScrollBar::MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	if(fContent!=nullptr){
		if(fButtonDown){
			if(fActiveButton==eScrollBarButton::Track){
				// move position
				Float32 dx=Pos.x-fTrackPos.x;
				Float32 dy=Pos.y-fTrackPos.y;
				Float32 ScrollOffset=fContent->ScrollOffsetToValue(dx,dy);
				if(ScrollOffset!=0){
					ScrollBarMove(ScrollOffset);
				}
			}

			fTrackPos=Pos;
		}
		else{
			fActiveButton=fContent->ScrollHitTest(Pos.x,Pos.y);
		}

		ScrollBarNotify();
	}
}
//---------------------------------------------------------------------------
void bcScrollBar::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	if(Button!=MouseButton::Left)
		return;
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);
	if(fContent!=nullptr){
		fActiveButton=fContent->ScrollHitTest(Pos.x,Pos.y);
		fButtonDown=true;

		fActiveMouse=MouseEvent->GetMouse();

		MousePress();

		
		uInt64 TimeNow=cnSystem::GetSystemTimeNow();
		fMousePressTimer->Start(TimeNow+300*Time_1ms,50*Time_1ms);
		
		if(fActiveButton==eScrollBarButton::Track){
			fTrackPos=Pos;
		}
		ScrollBarNotify();
	}
}
//---------------------------------------------------------------------------
void bcScrollBar::MouseUp(iUIMouseEvent*,eMouseButton Button)noexcept(true)
{
	if(Button!=MouseButton::Left)
		return;

	fActiveButton=eScrollBarButton::None;
	fButtonDown=false;
	fActiveMouse=nullptr;
	fMousePressTimer->Stop();
	ScrollBarNotify();
}
//---------------------------------------------------------------------------
void bcScrollBar::MouseWheel(iUIMouseEvent *MouseEvent,Float32 ScrollX,Float32 ScrollY)noexcept(true)
{
	Float32 ScrollOffset=fContent->ScrollOffsetToValue(ScrollX,ScrollY);
	if(ScrollOffset!=0){
		ScrollBarMove(ScrollOffset); 
	}
}
//---------------------------------------------------------------------------
void bcScrollBar::cMousePressTimerProc::Execute(void)noexcept(true)
{
	auto Host=cnMemory::GetObjectFromMemberPointer(this,&bcScrollBar::fMousePressTimerProc);
	Host->MousePress();
}
//---------------------------------------------------------------------------
void bcScrollBar::MousePress(void)noexcept(true)
{
	if(fActiveMouse==nullptr)
		return;
	
	if(fContent!=nullptr){
		cUIPoint Pos;
		fActiveMouse->GetPosition(fView,Pos);

		auto HitButton=fContent->ScrollHitTest(Pos.x,Pos.y);
		if(fActiveButton!=HitButton)
			return;
		switch(HitButton){
		default:
			break;
		case ScrollBarButton::Dec:
			ScrollBarMoveAction(true,sbMoveStep);
			break;
		case ScrollBarButton::Inc:
			ScrollBarMoveAction(false,sbMoveStep);
			break;
		case ScrollBarButton::DecBar:
			ScrollBarMoveAction(true,sbMovePage);
			break;
		case ScrollBarButton::IncBar:
			ScrollBarMoveAction(false,sbMovePage);
			break;
		}
		ScrollBarNotify();
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cScrollBar::cScrollBar()noexcept(true)
{
}
//---------------------------------------------------------------------------
cScrollBar::~cScrollBar()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cScrollBar::Update(void)noexcept(true)
{
	ScrollBarNotify();
}
//---------------------------------------------------------------------------
cScrollBar::cScrollPosition cScrollBar::ScrollBarPosition(void)noexcept(true)
{
	cScrollPosition ScrollPos;
	ScrollPos.View.Begin=ScrollCurrent;
	ScrollPos.View.End=ScrollCurrent+ScrollViewSize;
	ScrollPos.Total.Begin=ScrollMin;
	ScrollPos.Total.End=ScrollMax;
	return ScrollPos;
}
//---------------------------------------------------------------------------
void cScrollBar::ScrollBarMove(Float32 Offset)noexcept(true)
{
	if(Offset!=0){
		Float32 NewValue=ScrollCurrent+Offset;
		if(NewValue<ScrollMin){
			NewValue=ScrollMin;
		}
		else{
			Float32 LimitMax=ScrollMax-ScrollViewSize;
			if(NewValue>LimitMax)
				NewValue=LimitMax;
		}
		if(NewValue!=ScrollCurrent){
			ScrollCurrent=NewValue;
			if(OnScroll!=nullptr){
				OnScroll();
			}
		}

	}
}
//---------------------------------------------------------------------------
void cScrollBar::ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)noexcept(true)
{
	Float32 MoveOffset;
	switch(Distance){
	default:
	case sbMoveStep:
		MoveOffset=ScrollStepSize;
		break;
	case sbMovePage:
		MoveOffset=ScrollPageSize;
		break;

	}
	if(Negative){
		MoveOffset=-MoveOffset;
	}
	ScrollBarMove(MoveOffset);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cControlScrollBar::cControlScrollBar()noexcept(true)
{
}
//---------------------------------------------------------------------------
cControlScrollBar::~cControlScrollBar()noexcept(true)
{
	SetScrollInfo(nullptr);
}
//---------------------------------------------------------------------------
iControlScrollInfo* cControlScrollBar::GetScrollInfo(void)const noexcept(true)
{
	return fScrollInfo;
}
//---------------------------------------------------------------------------
void cControlScrollBar::SetScrollInfo(iControlScrollInfo *ScrollInfo)noexcept(true)
{
	if(fScrollInfo!=nullptr){
		fScrollInfo->ScrollInfoNotifySet.Remove(fScrollInfoNotifyToken);
		fScrollInfo->ScrollOffsetNotifySet.Remove(fScrollOffsetNotifyToken);
	}
	fScrollInfo=ScrollInfo;
	if(fScrollInfo!=nullptr){
		fScrollInfoNotifyToken=fScrollInfo->ScrollInfoNotifySet.Insert([this]{
			ScrollInfoChanged();
		});
		fScrollOffsetNotifyToken=fScrollInfo->ScrollOffsetNotifySet.Insert([this]{
			ScrollInfoChanged();
		});
	}

	ScrollInfoChanged();
}
//---------------------------------------------------------------------------
void cControlScrollBar::ScrollInfoChanged(void)noexcept(true)
{
	ScrollBarNotify();
}
//---------------------------------------------------------------------------
bool cControlScrollBar::NeedScrollBar(void)const noexcept(true)
{
	if(fScrollInfo==nullptr)
		return false;

	auto ScrollRange=fScrollInfo->ScrollGetScrollRange();
	if(ScrollRange.NoLowwerLimit || ScrollRange.NoUpperLimit)
		return true;

	Float32 TotalSize=ScrollRange.UpperLimit-ScrollRange.LowwerLimit;
	return TotalSize>ScrollRange.ViewportSize;
}
//---------------------------------------------------------------------------
cControlScrollBar::cScrollPosition cControlScrollBar::ScrollBarPosition(void)noexcept(true)
{
	cScrollPosition ScrollPos;
	if(fScrollInfo!=nullptr){
		auto ScrollRange=fScrollInfo->ScrollGetScrollRange();
		ScrollPos.View.Begin=fScrollInfo->ScrollGetOffset();
		ScrollPos.View.End=ScrollRange.ViewportSize;
		ScrollPos.View.End+=ScrollPos.View.Begin;
		ScrollPos.Total.Begin=ScrollRange.LowwerLimit;
		ScrollPos.Total.End=ScrollRange.UpperLimit;
	}
	else{
		ScrollPos.View.Begin=0;
		ScrollPos.View.End=0;
		ScrollPos.Total.Begin=0;
		ScrollPos.Total.End=0;
	}
	return ScrollPos;
}
//---------------------------------------------------------------------------
void cControlScrollBar::ScrollBarMove(Float32 Offset)noexcept(true)
{
	if(fScrollInfo==nullptr){
		return;
	}

	if(Offset!=0){
		Float32 NewValue=fScrollInfo->ScrollGetOffset();
		NewValue+=Offset;
		fScrollInfo->ScrollSetOffset(NewValue);
	}
}
//---------------------------------------------------------------------------
void cControlScrollBar::ScrollBarMoveAction(bool Negative,eScrollBarMoveDistance Distance)noexcept(true)
{
	if(fScrollInfo==nullptr){
		return;
	}

	Float32 MoveOffset;
	switch(Distance){
	default:
	case sbMoveStep:
		MoveOffset=fScrollInfo->ScrollGetStepSize();
		break;
	case sbMovePage:
		MoveOffset=fScrollInfo->ScrollGetPageSize();
		break;

	}
	if(Negative){
		MoveOffset=-MoveOffset;
	}
	ScrollBarMove(MoveOffset);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcTab::bcTab()noexcept(true)
{
}
//---------------------------------------------------------------------------
bcTab::~bcTab()noexcept(true)
{
}
//---------------------------------------------------------------------------
sfInt16 bcTab::TabHitTest(Float32 x,Float32 y)noexcept(true)
{
	auto ViewSize=fView->GetSize();
	if(y<0)
		return -1;
	if(y>ViewSize.y)
		return -1;
	Float32 TabRight=ViewSize.x;
	if(x>TabRight)
		return -1;
	if(x<0)
		return -1;
	Float32 TabLeft=0;
	uIntn TabCount=TabGetCount();
	for(uIntn i=0;i<TabCount;i++){
		sfInt32 TabSize=TabGetSize(i);
		if(x<TabLeft+TabSize){
			return static_cast<sfInt16>(i);
		}
		TabLeft+=TabSize;
	}
	return -1;
}
//---------------------------------------------------------------------------
void bcTab::MouseLeave(iUIMouseEvent*)noexcept(true)
{
	fMouseBtnLeftDown=false;

	TabUpdateHotIndex(-1);
}
//---------------------------------------------------------------------------
void bcTab::MouseMove(iUIMouseEvent *Mouse)noexcept(true)
{
	if(fMouseBtnLeftDown==false){
		cUIPoint Pos;
		Mouse->GetPosition(fView,Pos);

		sfInt16 HotIndex=TabHitTest(Pos.x,Pos.y);
		TabUpdateHotIndex(HotIndex);
	}
}
//---------------------------------------------------------------------------
void bcTab::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	if(fMouseBtnLeftDown==false){
		if(Button==MouseButton::Left){
			cUIPoint Pos;
			MouseEvent->GetPosition(fView,Pos);

			fMouseBtnLeftDown=true;
			fMouseDownIndex=TabHitTest(Pos.x,Pos.y);
			TabUpdateHotIndex(fMouseDownIndex);
		}
	}
}
//---------------------------------------------------------------------------
void bcTab::MouseUp(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	cUIPoint Pos;
	MouseEvent->GetPosition(fView,Pos);

	if(fMouseBtnLeftDown && Button==MouseButton::Left){
		// test if is valid click
		sfInt16 MouseUpIndex=TabHitTest(Pos.x,Pos.y);
		TabUpdateHotIndex(MouseUpIndex);
		if(MouseUpIndex==fMouseDownIndex){
			// click event
			TabClick(fMouseDownIndex);
		}
		fMouseBtnLeftDown=false;
	}
}
//---------------------------------------------------------------------------
sfInt16 bcTab::TabGetCount(void)noexcept(true){	return 0;	}
sfInt32 bcTab::TabGetSize(sfInt16 Index)noexcept(true){	return 1;}
void bcTab::TabUpdateHotIndex(sfInt16 Index)noexcept(true){}
void bcTab::TabClick(sfInt16)noexcept(true){}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rPtr<viControl> (*cTextTab::CreateDefaultVisual)(iVisualData<cTextTabData> *Data)noexcept(true)=nullptr;
//---------------------------------------------------------------------------
cTextTab::cTextTab()noexcept(true)
	: fDataCache(rCreate< cUIVisualDataCache<cTextTabData> >(cnSystem::CurrentUIThread::GetUIThread(),static_cast<cUIVisualDataCache<cTextTabData>::iDataProvider*>(this)))
{
	gApplyDefaultTextStyle(TextStyle);
}
//---------------------------------------------------------------------------
cTextTab::~cTextTab()noexcept(true)
{
	fDataCache->ProviderClosed();
}
//---------------------------------------------------------------------------
cTextTab::operator iVisualData<cTextTabData> *()const noexcept(true)
{
	return fDataCache;
}
//---------------------------------------------------------------------------
void cTextTab::ControlContentSetDefault(void)noexcept(true)
{
	if(CreateDefaultVisual!=nullptr){
		SetContent(CreateDefaultVisual(*this));
	}
}
//---------------------------------------------------------------------------
sfInt16 cTextTab::TabGetCount(void)noexcept(true)
{
	return static_cast<sfInt16>(fTabList.GetCount());
}
//---------------------------------------------------------------------------
sfInt32 cTextTab::TabGetSize(sfInt16 Index)noexcept(true)
{
	auto &TabItem=fTabList[Index];
	return TabItem.TabSize;
}
//---------------------------------------------------------------------------
void cTextTab::Update(void)noexcept(true)
{
	fDataCache->NotifyUpdate();
}
//---------------------------------------------------------------------------
const cTextTabData* cTextTab::QueryData(rPtr<iReference> &DataReference)noexcept(true)
{
	auto DataRef=rCreate<cDataReference>();
	auto &Data=DataRef->Data;

	DataRef->ItemList=fTabList;

	DataRef->Data.ActiveIndex=fActiveIndex;
	DataRef->Data.HotIndex=fHotIndex;
	DataRef->Data.TabItems=DataRef->ItemList.Storage();

	DataReference=DataRef;
	return &Data;
}
//---------------------------------------------------------------------------
sfInt16 cTextTab::GetActiveIndex(void)const noexcept(true)
{
	return fActiveIndex;
}
//---------------------------------------------------------------------------
void cTextTab::SetActiveIndex(sfInt16 Index)noexcept(true)
{
	if(fActiveIndex!=Index){
		fActiveIndex=Index;
		fDataCache->NotifyUpdate();
		if(OnTabChanged!=nullptr){
			OnTabChanged();
		}
	}
}
//---------------------------------------------------------------------------
void cTextTab::ClearTab(void)noexcept(true)
{
	fTabList.Clear();
}
//---------------------------------------------------------------------------
void cTextTab::SetTab(uIntn Index,const cTextTabItem &Item)noexcept(true)
{
	if(fTabList.GetCount()<=Index){
		fTabList.SetCount(Index+1);
	}

	auto &TabItem=fTabList[Index];
	TabItem=Item;
	fDataCache->NotifyUpdate();
}
//---------------------------------------------------------------------------
void cTextTab::TabUpdateHotIndex(sfInt16 Index)noexcept(true)
{
	if(fHotIndex==Index)
		return;

	fHotIndex=Index;
	fDataCache->NotifyUpdate();
}
//---------------------------------------------------------------------------
void cTextTab::TabClick(sfInt16 Index)noexcept(true)
{
	if(OnClickTab!=nullptr){
		OnClickTab(Index);
	}
	else{
		// default action : change active tab
		if(Index!=-1){
			SetActiveIndex(Index);
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cSplitterBar::cSplitterBar()noexcept(true)
{
	MouseCursorH=cnSystem::GetSysMouseCursor(SysMouseCursor::SizeH);
	MouseCursorV=cnSystem::GetSysMouseCursor(SysMouseCursor::SizeV);
}
//---------------------------------------------------------------------------
cSplitterBar::~cSplitterBar()noexcept(true)
{
}
//---------------------------------------------------------------------------
void cSplitterBar::ControlContentSetDefault(void)noexcept(true)
{
	if(gCreateDefaultSplitterBarVisual!=nullptr){
		SetContent(gCreateDefaultSplitterBarVisual());
	}
}
//---------------------------------------------------------------------------
void cSplitterBar::RectangleChanged(bool,bool Sized)noexcept(true)
{
	if(Sized){
		SizeChanged();
	}
}
//---------------------------------------------------------------------------
void cSplitterBar::SizeChanged(void)noexcept(true)
{
	auto Size=fView->GetSize();
	bool Horization=Size.x<Size.y;
	if(Horization!=fHorization){
		fHorization=Horization;
		auto MouseControl=iCast<iUIMouseControl>(fView);
		if(MouseControl!=nullptr){
			MouseControl->MouseUpdateCursor();
		}
	}
}
//---------------------------------------------------------------------------
void cSplitterBar::MouseLeave(iUIMouseEvent*)noexcept(true)
{
	fBtnDown=false;
}
//---------------------------------------------------------------------------
iInterface* cSplitterBar::MouseGetCursor(iUIMouseEvent*)noexcept(true)
{
	if(fHorization){
		return MouseCursorH;
	}
	else{
		return MouseCursorV;
	}
}
//---------------------------------------------------------------------------
void cSplitterBar::MouseMove(iUIMouseEvent *MouseEvent)noexcept(true)
{
	if(fBtnDown){
		cUIPoint CurPos;
		MouseEvent->GetPosition(fView,CurPos);
		Float32 Delta;
		if(fHorization){
			Delta=CurPos.x-fMoveRefPoint.x;
		}
		else{
			Delta=CurPos.y-fMoveRefPoint.y;
		}
		if(OnDrag!=nullptr){
			OnDrag(Delta);
		}
	}
}
//---------------------------------------------------------------------------
void cSplitterBar::MouseDown(iUIMouseEvent *MouseEvent,eMouseButton Button)noexcept(true)
{
	if(Button==MouseActionButton){
		fBtnDown=true;
		MouseEvent->GetPosition(fView,fMoveRefPoint);
	}
}
//---------------------------------------------------------------------------
void cSplitterBar::MouseUp(iUIMouseEvent*,eMouseButton Button)noexcept(true)
{
	if(Button==MouseActionButton){
		fBtnDown=false;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
lEdgeInsets::lEdgeInsets()noexcept(true)
{
}
//---------------------------------------------------------------------------
lEdgeInsets::~lEdgeInsets()noexcept(true)
{
}
//---------------------------------------------------------------------------
void lEdgeInsets::ViewSetup(void)noexcept(true)
{
	LayoutControl::ViewSetup();

	for(uIntn i=0;i<4;i++){
		auto &Inset=fInsets[i];
		if(Inset.View!=nullptr){
			if(Inset.Content!=nullptr){
				fInsets[i].Content->SetView(Inset.View);
			}
			fView->InsertView(Inset.View);
		}
	}
}
//---------------------------------------------------------------------------
void lEdgeInsets::ViewClear(void)noexcept(true)
{
	for(uIntn i=0;i<4;i++){
		auto &Inset=fInsets[i];
		if(Inset.Content!=nullptr){
			Inset.Content->SetView(nullptr);
		}
		if(Inset.View!=nullptr){
			fView->RemoveView(Inset.View);
		}
	}

	LayoutControl::ViewClear();
}
//---------------------------------------------------------------------------
iControlComponent* lEdgeInsets::GetInset(eDirection Direction)const noexcept(true)
{
	return fInsets[Direction].Content;
}
//---------------------------------------------------------------------------
void lEdgeInsets::SetInset(rPtr<iControlComponent> Inset,eDirection Direction)noexcept(true)
{
	auto &InsetItem=fInsets[Direction];
	if(InsetItem.Content!=nullptr){
		InsetItem.Content->SetView(nullptr);
	}
	InsetItem.Content=cnVar::MoveCast(Inset);
	if(InsetItem.Content!=nullptr){
		if(InsetItem.View==nullptr){
			InsetItem.View=cnSystem::CreateUIView();
			if(fView!=nullptr){
				fView->InsertView(InsetItem.View);
			}
		}
		InsetItem.Content->SetView(InsetItem.View);
	}

}
//---------------------------------------------------------------------------
cUIRectangle lEdgeInsets::UIMargin(const cUIRectangle &Margin)noexcept(true)
{
	fInsetPos=Margin;

	cUIRectangle InsetMargin=UIRectangleZero;
	if(fInsets[Direction::Left].Content!=nullptr){
		InsetMargin.Left=InsetSizes.Left;
	}
	if(fInsets[Direction::Up].Content!=nullptr){
		InsetMargin.Top=InsetSizes.Top;
	}
	if(fInsets[Direction::Right].Content!=nullptr){
		InsetMargin.Right=InsetSizes.Right;
	}
	if(fInsets[Direction::Down].Content!=nullptr){
		InsetMargin.Bottom=InsetSizes.Bottom;
	}

	return InsetMargin;
}
//---------------------------------------------------------------------------
void lEdgeInsets::UILayout(void)noexcept(true)
{
	auto ViewSize=fView->GetSize();

	Float32 InsetWidth=ViewSize.x-fInsetPos.Right-fInsetPos.Left;
	Float32 InsetHeight=ViewSize.y-fInsetPos.Top-fInsetPos.Bottom;

	if(fInsets[Direction::Left].View!=nullptr){
		fInsets[Direction::Left].View->ArrangeRectangle(fView,{fInsetPos.Left,fInsetPos.Top},{InsetSizes.Left,InsetHeight});
	}
	if(fInsets[Direction::Up].View!=nullptr){
		fInsets[Direction::Up].View->ArrangeRectangle(fView,{fInsetPos.Left,fInsetPos.Top},{InsetWidth,InsetSizes.Top});
	}
	if(fInsets[Direction::Right].View!=nullptr){
		fInsets[Direction::Right].View->ArrangeRectangle(fView,{fInsetPos.Right-InsetSizes.Right,fInsetPos.Top},{InsetSizes.Right,InsetHeight});
	}
	if(fInsets[Direction::Down].View!=nullptr){
		fInsets[Direction::Down].View->ArrangeRectangle(fView,{fInsetPos.Left,fInsetPos.Bottom-InsetSizes.Bottom},{InsetWidth,InsetSizes.Bottom});
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cTitledFrame::cTitledFrame()noexcept(true)
{
	gApplyDefaultTextStyle(TextStyle);

	TextMargin=UIRectangleZero;
}
//---------------------------------------------------------------------------
cTitledFrame::~cTitledFrame()noexcept(true)
{
	viTextControlData::InvalidateData();
}
//---------------------------------------------------------------------------
void cTitledFrame::Update(void)noexcept(true)
{
	ControlTextNotifySet();
}
//---------------------------------------------------------------------------
const uChar16* cTitledFrame::ControlTextString(void)noexcept(true)
{
	return Text->Pointer;
}
//---------------------------------------------------------------------------
uIntn cTitledFrame::ControlTextLength(void)noexcept(true)
{
	return Text->Length;
}
//---------------------------------------------------------------------------
const cUITextStyle& cTitledFrame::ControlTextStyle(void)noexcept(true)
{
	return TextStyle;
}
//---------------------------------------------------------------------------
cUIRectangle cTitledFrame::ControlTextMargin(void)noexcept(true)
{
	return TextMargin;
}
//---------------------------------------------------------------------------
eAlignment cTitledFrame::ControlTextAlignment(void)noexcept(true)
{
	return Alignment::Center;
}
//---------------------------------------------------------------------------
