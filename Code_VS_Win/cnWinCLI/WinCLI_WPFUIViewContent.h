/*- cnWinWPF - View Content -----------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2023-10-07                                          */
/*-------------------------------------------------------------------------*/
#pragma once
/*-------------------------------------------------------------------------*/

#include <cnWin\Win_GDI.h>

#include <cnWinCLI\WinDNet_Common.h>
#include <cnWinCLI\WinDNet_WPFUIView.h>

#include <cnWinCLI\WinDNetM_WPFUIViewContent.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnWin{
//---------------------------------------------------------------------------
class cnLib_INTERFACE miWPFUISimpleGraph : public iInterface
{
public:
	struct tInterfaceID{	static iTypeID Value;	};
	virtual void* cnLib_FUNC CastInterface(iTypeID ID)noexcept(true) override{	return cnVar::ImplementCastInterface(this,ID);	}

	virtual void Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)=0;
};
//---------------------------------------------------------------------------
iPtr<iUIFont> DNetCreateFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight)noexcept(true);
rPtr<iUITextLineLayout> DNetCreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style)noexcept(true);
//---------------------------------------------------------------------------
class cWPFUIBitmap : public iUIBitmap, public miWPFUISimpleGraph
{
public:
	cWPFUIBitmap(mcWPFUIBitmap &&Bitmap)noexcept(true);
	~cWPFUIBitmap()noexcept(true);

	using miWPFUISimpleGraph::CastInterface;

	// iUIBitmap
	
	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)override;

protected:
	mcWPFUIBitmap fBitmap;
};
//---------------------------------------------------------------------------
class cWPFBitmapDataSource : public iBitmapDataSource, public miWPFUISimpleGraph
{
public:
	cWPFBitmapDataSource(mcWPFBitmapDataSource &&Source)noexcept(true);
	~cWPFBitmapDataSource()noexcept(true);

	using miWPFUISimpleGraph::CastInterface;

	// iBitmapDataSource
	
	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)override;

protected:
	mcWPFBitmapDataSource fBitmapSource;
};
//---------------------------------------------------------------------------
class cWPFUISimpleBitmapCanvas : public iUISimpleBitmapCanvas, public mcWPFUISimpleBitmapCanvas
{
public:
	cWPFUISimpleBitmapCanvas(cUIPoint Size)noexcept(true);
	~cWPFUISimpleBitmapCanvas()noexcept(true);

	// iUISimpleBitmapCanvas

	virtual iUISimplePaintContext* cnLib_FUNC StartContext(cUIColor Color)noexcept(true)override;
	virtual void cnLib_FUNC DiscardContext(void)noexcept(true)override;
	virtual iPtr<iUIBitmap> cnLib_FUNC FinishBitmap(bool CopyContent)noexcept(true)override;
	virtual iPtr<iBitmapDataSource> cnLib_FUNC FinishBitmapSource(bool CopyContent)noexcept(true)override;

protected:
	mcWPFDrawingContext fContext;
};
//---------------------------------------------------------------------------
class cWPFTypeface : public iUIFont, public iCLIObject
{
public:
	cWPFTypeface(mcWPFTypeface &&Typeface)noexcept(true);
	~cWPFTypeface()noexcept(true);

	using iCLIObject::CastInterface;

	// iUIFont

	virtual cArray<const uChar16> cnLib_FUNC GetName(void)noexcept(true)override;
	virtual eUIFontStyle cnLib_FUNC GetStyle(void)noexcept(true)override;

	virtual Float32 cnLib_FUNC GetWeight(void)noexcept(true)override;

	// iCLIObject

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)noexcept(true)override;
private:
	mcWPFTypeface fTypeface;
};
//---------------------------------------------------------------------------
class cWPFTextLayout : public iUITextLineLayout, public mcWPFTextLayout
{
public:
	virtual uIntn cnLib_FUNC GetTextLength(void)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetTextHeight(void)noexcept(true)override;
	virtual const Float32* cnLib_FUNC GetTextDistance(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWPFUISimpleTextGraph : public iUITextGraph, public miWPFUISimpleGraph, public mcWPFUISimpleTextGraph
{
public:
	cWPFUISimpleTextGraph()noexcept(true);
	~cWPFUISimpleTextGraph()noexcept(true);

	using miWPFUISimpleGraph::CastInterface;

	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWPFUIRichTextGraph : public iUITextGraph, public miWPFUISimpleGraph, public mcWPFUIRichTextGraph
{
public:
	cWPFUIRichTextGraph()noexcept(true);
	~cWPFUIRichTextGraph()noexcept(true);

	using miWPFUISimpleGraph::CastInterface;

	virtual cUIPoint cnLib_FUNC GetSize(void)noexcept(true)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cWPFUISimplePaintDevice : public iUISimplePaintDevice
{
public:
	cWPFUISimplePaintDevice()noexcept(true);
	~cWPFUISimplePaintDevice()noexcept(true);

	virtual rPtr<iUISimpleBitmapCanvas> cnLib_FUNC CreateBitmapCanvas(cUIPoint Size)noexcept(true)override;
	virtual iPtr<iUITextGraph> cnLib_FUNC CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)noexcept(true)override;

	virtual iPtr<iUIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)noexcept(true)override;

};
//---------------------------------------------------------------------------
class cWPFUISimpleViewContent : public iUISimpleViewContent, public bcWPFViewContentDrawing
{
public:
	cWPFUISimpleViewContent(iUISimplePainter *Painter)noexcept(true);
	~cWPFUISimpleViewContent()noexcept(true);


	// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)noexcept(true)override;
	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)noexcept(true)override;

	// iUISimpleViewContent

	virtual rPtr<iUISimplePaintDevice> cnLib_FUNC QueryDevice(void)noexcept(true)override;
	virtual eUIState cnLib_FUNC GetPaintState(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)noexcept(true)override;
	virtual void cnLib_FUNC QueryUpdate(void)noexcept(true)override;


	// mbcWPFViewContentDrawing

	virtual cGCRef* GetDrawingPointer(void)noexcept(true)override;

	virtual void ViewContentUpdateRect(void)noexcept(true)override;
	virtual void ViewContentUpdateState(void)noexcept(true)override;
	virtual void ViewContentUpdateWindow(iWindow *Window)noexcept(true)override;

protected:
	mcWPFUIViewContentDrawingGroup fWPFDrawing;
	iUISimplePainter *fPainter;
	cUIPoint fViewContentSize;

	eUIState fPainterState;
	eUIState CheckPainterState(void)noexcept(true);
	void UpdatePainterState(void)noexcept(true);
	static void NotifyPainterState(iUISimplePainter *Painter,eUIState NewState,eUIState OldState)noexcept(true);

	void RenderVisual(void)noexcept(true);
};
//---------------------------------------------------------------------------
struct cDNetGDIThreadCPPContext
{
	cDNetGDIThreadCPPContext()noexcept(true);
	~cDNetGDIThreadCPPContext()noexcept(true);

	rPtr<cnRTL::cnWinRTL::cDCPaintDevice> Device;

	void NotifyDisplayChanged(void)noexcept(true);

	static cDNetGDIThreadCPPContext* mThreadContext(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cWPFDCViewContent : public iDCViewContent, public bcWPFViewContentDrawing
{
public:
	cWPFDCViewContent(iDCPainter *Painter,bool OutputAlpha)noexcept(true);
	~cWPFDCViewContent()noexcept(true);

	virtual cGCRef* GetDrawingPointer(void)noexcept(true)override;

	// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)noexcept(true)override;
	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)noexcept(true)override;

	// iUIDCViewContent

	virtual eUIState cnLib_FUNC GetPaintState(void)noexcept(true)override;
	virtual RECT cnLib_FUNC GetPaintRect(void)noexcept(true)override;
	virtual HDC	cnLib_FUNC GetDC(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleaseDC(HDC DC)noexcept(true)override;
	virtual HDC	cnLib_FUNC GetIC(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleaseIC(HDC DC)noexcept(true)override;
	virtual void cnLib_FUNC InvalidateRect(const RECT *Rect)noexcept(true)override;
	virtual iWindow* cnLib_FUNC GetWindow(void)noexcept(true)override;

protected:
private:

	mcWPFImageDrawing fDrawing;

	struct cContentDC
	{
		uIntn RefCount;
		HDC MemDC;
		HGDIOBJ MemDCBitmap;
		cnRTL::cnWinRTL::cGDIBitmapBuffer Buffer;
	};
	cContentDC *fContentDC=nullptr;
	rPtr<cnRTL::cnWinRTL::cGDIDC> fTempIC;
	uIntn fICRef;
	cnRTL::cnWinRTL::cGDIBitmapSection fBitmapBuffer;

	iDCPainter *fPainter;
	iWindow *fWindow=nullptr;
	int fDrawWidth;
	int fDrawHeight;

	virtual void ViewContentUpdateRect(void)noexcept(true)override;
	virtual void ViewContentUpdateState(void)noexcept(true)override;
	virtual void ViewContentUpdateWindow(iWindow *Window)noexcept(true)override;

	enum ePainterState
	{
		psNone,
		psBackground,
		psActive
	};
	ePainterState fPainterState;
	ePainterState CheckPainterState(void)noexcept(true);
	static void NotifyPainterState(iDCPainter *Painter,ePainterState NewState,ePainterState OldState)noexcept(true);
	void UpdatePainterState(void)noexcept(true);

	bool fNeedPaint=false;
	bool fNeedNotifyRect=false;
	void Paint(void)noexcept(true);
	void SetNeedPaint(void)noexcept(true);



	bool fPainterOutputAlpha;
	void CPPPaintStart(void *BitmapData,int Width,int Height)noexcept(true);
	void CPPPaintFinish(void *BitmapData,int Width,int Height)noexcept(true);
	
	void SetupBitmap(HDC DC)noexcept(true);
	void CPPPaint(cDNetGDIThreadCPPContext *CPP)noexcept(true);
};
//---------------------------------------------------------------------------
class cD3D9ExWindowDevice : public iReference, public iWindowMessageHandler
{
public:
	cD3D9ExWindowDevice(iWindow *Window)noexcept(true);
	~cD3D9ExWindowDevice()noexcept(true);

	IDirect3DDevice9Ex* GetDevice(void)const noexcept(true);

	virtual void cnLib_FUNC WindowAttached(void)noexcept(true)override;
	virtual void cnLib_FUNC WindowDetached(void)noexcept(true)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)noexcept(true)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)noexcept(true)override;

protected:

	iWindow *fWindow;
	COMPtr<IDirect3DDevice9Ex> fDevice;
	static const D3DPRESENT_PARAMETERS DefaultPresentParameter;
};
//---------------------------------------------------------------------------
class cWPFDXGIViewContent : public iDXGIViewContent, public bcWPFViewContentDrawing
{
public:
	cWPFDXGIViewContent(iDXGIPainter *Painter)noexcept(true);
	~cWPFDXGIViewContent()noexcept(true);

	virtual cGCRef* GetDrawingPointer(void)noexcept(true)override;

	// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)noexcept(true)override;
	virtual bool cnLib_FUNC GetVisible(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)noexcept(true)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)noexcept(true)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)noexcept(true)override;

	// iUIDCViewContent

	virtual eUIState cnLib_FUNC GetPaintState(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)noexcept(true)override;
	virtual bool GetRenderBufferSharedHandle(HANDLE &SharedHandle)noexcept(true)override;
	virtual void UpdateRenderBuffer(void)noexcept(true)override;

protected:
private:

	mcWPFD3DImageDrawing fDrawing;

	iDXGIPainter *fPainter;
	iWindow *fWindow=nullptr;
	int fDrawWidth;
	int fDrawHeight;

	virtual void ViewContentUpdateRect(void)noexcept(true)override;
	virtual void ViewContentUpdateState(void)noexcept(true)override;
	virtual void ViewContentUpdateWindow(iWindow *Window)noexcept(true)override;

	enum ePainterState
	{
		psNone,
		psBackground,
		psActive
	};
	ePainterState fPainterState;
	ePainterState CheckPainterState(void)noexcept(true);
	static void NotifyPainterState(iDXGIPainter *Painter,ePainterState NewState,ePainterState OldState)noexcept(true);
	void UpdatePainterState(void)noexcept(true);

	template<class T>
	class cAffixedVariable
	{
	public:
		template<class THost>
		void Affix(const THost &Host)noexcept(true){
			fVariable=Host->QueryAffixedVariable(cnVar::TTypeID<T>::Value);
			fValue=static_cast<T*>(fVariable->Get());
			if(fValue==nullptr){
				auto NewContent=cnRTL::rCreate<T>(Host);
				fValue=NewContent;
				fVariable->Set(cnVar::MoveCast(NewContent),fValue);
			}
		}
		void Clear(void)noexcept(true){
			fVariable=nullptr;
			fValue=nullptr;
		}

		operator T*()const noexcept(true){	return fValue;	}
		T* operator -> ()const noexcept(true){	return fValue;	}
	protected:
		rPtr<iVariable> fVariable;
		T *fValue;
	};

	cAffixedVariable<cD3D9ExWindowDevice> fD3D9ExDevice;

	COMPtr<IDirect3DSurface9> fSurface;
	HANDLE fSurfaceSharedHandle;

	void FrontAvailableChanged(void)noexcept(true);
	class cFrontAvailableChangedProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)noexcept(true)override;
	}fFrontAvailableChangedProcedure;

	void SetupBackBuffer(void)noexcept(true);
	void ClearBackBuffer(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
