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
	cnLib_INTERFACE_DEFINE(miWPFUISimpleGraph,iInterface)

	virtual void Draw(cGCRef &ContextHandle,cUIPoint DrawPosition,cUIPoint DrawSize)=0;
};
//---------------------------------------------------------------------------
iPtr<iUIFont> DNetCreateFont(const uChar16 *Name,uIntn NameLength,eUIFontStyle FontStyle,Float32 FontWeight);
rPtr<iUITextLineLayout> DNetCreateTextLineLayout(const uChar16 *Text,uIntn Length,const cUITextStyle &Style);
//---------------------------------------------------------------------------
class cWPFUIBitmap : public iUIBitmap, public miWPFUISimpleGraph
{
public:
	cWPFUIBitmap(mcWPFUIBitmap &&Bitmap);
	~cWPFUIBitmap();

	using miWPFUISimpleGraph::CastInterface;

	// iUIBitmap
	
	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)override;

protected:
	mcWPFUIBitmap fBitmap;
};
//---------------------------------------------------------------------------
class cWPFBitmapDataSource : public iBitmapDataSource, public miWPFUISimpleGraph
{
public:
	cWPFBitmapDataSource(mcWPFBitmapDataSource &&Source);
	~cWPFBitmapDataSource();

	using miWPFUISimpleGraph::CastInterface;

	// iBitmapDataSource
	
	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;
	virtual bool cnLib_FUNC IsTopDown(void)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)override;

protected:
	mcWPFBitmapDataSource fBitmapSource;
};
//---------------------------------------------------------------------------
class cWPFUISimpleBitmapCanvas : public iUISimpleBitmapCanvas, public mcWPFUISimpleBitmapCanvas
{
public:
	cWPFUISimpleBitmapCanvas(cUIPoint Size);
	~cWPFUISimpleBitmapCanvas();

	// iUISimpleBitmapCanvas

	virtual iUISimplePaintContext* cnLib_FUNC StartContext(cUIColor Color)override;
	virtual void cnLib_FUNC DiscardContext(void)override;
	virtual iPtr<iUIBitmap> cnLib_FUNC FinishBitmap(bool CopyContent)override;
	virtual iPtr<iBitmapDataSource> cnLib_FUNC FinishBitmapSource(bool CopyContent)override;

protected:
	mcWPFDrawingContext fContext;
};
//---------------------------------------------------------------------------
class cWPFTypeface : public iUIFont, public iCLIObject
{
public:
	cWPFTypeface(mcWPFTypeface &&Typeface);
	~cWPFTypeface();

	using iCLIObject::CastInterface;

	// iUIFont

	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)override;
	virtual eUIFontStyle cnLib_FUNC GetStyle(void)override;

	virtual Float32 cnLib_FUNC GetWeight(void)override;

	// iCLIObject

	virtual cGCRef& cnLib_FUNC GetObjecHandle(void)override;
private:
	mcWPFTypeface fTypeface;
};
//---------------------------------------------------------------------------
class cWPFTextLayout : public iUITextLineLayout, public mcWPFTextLayout
{
public:
	virtual uIntn cnLib_FUNC GetTextLength(void)override;
	virtual Float32 cnLib_FUNC GetTextHeight(void)override;
	virtual const Float32* cnLib_FUNC GetTextDistance(void)override;
};
//---------------------------------------------------------------------------
class cWPFUISimpleTextGraph : public iUITextGraph, public miWPFUISimpleGraph, public mcWPFUISimpleTextGraph
{
public:
	cWPFUISimpleTextGraph();
	~cWPFUISimpleTextGraph();

	using miWPFUISimpleGraph::CastInterface;

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)override;
};
//---------------------------------------------------------------------------
class cWPFUIRichTextGraph : public iUITextGraph, public miWPFUISimpleGraph, public mcWPFUIRichTextGraph
{
public:
	cWPFUIRichTextGraph();
	~cWPFUIRichTextGraph();

	using miWPFUISimpleGraph::CastInterface;

	virtual cUIPoint cnLib_FUNC GetSize(void)override;

	virtual void Draw(cGCRef &Handle,cUIPoint DrawPosition,cUIPoint DrawSize)override;
};
//---------------------------------------------------------------------------
class cWPFUISimplePaintDevice : public iUISimplePaintDevice
{
public:
	cWPFUISimplePaintDevice();
	~cWPFUISimplePaintDevice();

	virtual rPtr<iUISimpleBitmapCanvas> cnLib_FUNC CreateBitmapCanvas(cUIPoint Size)override;
	virtual iPtr<iUITextGraph> cnLib_FUNC CreateTextGraph(const uChar16 *Text,uIntn Length,const cUITextStyle &Style,const Float32* TextDistance,Float32 TextMinHeight)override;

	virtual iPtr<iUIBitmap> cnLib_FUNC CreateBitmapCopyFromSource(iBitmapDataSource *Source)override;

};
//---------------------------------------------------------------------------
class cWPFUISimpleViewContent : public iUISimpleViewContent, public bcWPFViewContentDrawing
{
public:
	cWPFUISimpleViewContent(iUISimplePainter *Painter);
	~cWPFUISimpleViewContent();


	// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;

	// iUISimpleViewContent

	virtual rPtr<iUISimplePaintDevice> cnLib_FUNC QueryDevice(void)override;
	virtual eUIState cnLib_FUNC GetPaintState(void)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)override;
	virtual void cnLib_FUNC QueryUpdate(void)override;


	// mbcWPFViewContentDrawing

	virtual cGCRef* GetDrawingPointer(void)override;

	virtual void ViewContentUpdateRect(void)override;
	virtual void ViewContentUpdateState(void)override;
	virtual void ViewContentUpdateWindow(iWindow *Window)override;

protected:
	mcWPFUIViewContentDrawingGroup fWPFDrawing;
	iUISimplePainter *fPainter;
	cUIPoint fViewContentSize;

	eUIState fPainterState;
	eUIState CheckPainterState(void);
	void UpdatePainterState(void);
	static void NotifyPainterState(iUISimplePainter *Painter,eUIState NewState,eUIState OldState);

	void RenderVisual(void);
};
//---------------------------------------------------------------------------
struct cDNetGDIThreadCPPContext
{
	cDNetGDIThreadCPPContext();
	~cDNetGDIThreadCPPContext();

	rPtr<cnRTL::cnWinRTL::cDCPaintDevice> Device;

	void NotifyDisplayChanged(void);

	static cDNetGDIThreadCPPContext* mThreadContext(void);
};
//---------------------------------------------------------------------------
class cWPFDCViewContent : public iDCViewContent, public bcWPFViewContentDrawing
{
public:
	cWPFDCViewContent(iDCPainter *Painter,bool OutputAlpha);
	~cWPFDCViewContent();

	virtual cGCRef* GetDrawingPointer(void)override;

	// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;

	// iUIDCViewContent

	virtual eUIState cnLib_FUNC GetPaintState(void)override;
	virtual RECT cnLib_FUNC GetPaintRect(void)override;
	virtual HDC	cnLib_FUNC GetDC(void)override;
	virtual void cnLib_FUNC ReleaseDC(HDC DC)override;
	virtual HDC	cnLib_FUNC GetIC(void)override;
	virtual void cnLib_FUNC ReleaseIC(HDC DC)override;
	virtual void cnLib_FUNC InvalidateRect(const RECT *Rect)override;
	virtual iWindow* cnLib_FUNC GetWindow(void)override;

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

	virtual void ViewContentUpdateRect(void)override;
	virtual void ViewContentUpdateState(void)override;
	virtual void ViewContentUpdateWindow(iWindow *Window)override;

	enum ePainterState
	{
		psNone,
		psBackground,
		psActive
	};
	ePainterState fPainterState;
	ePainterState CheckPainterState(void);
	static void NotifyPainterState(iDCPainter *Painter,ePainterState NewState,ePainterState OldState);
	void UpdatePainterState(void);

	bool fNeedPaint=false;
	bool fNeedNotifyRect=false;
	void Paint(void);
	void SetNeedPaint(void);



	bool fPainterOutputAlpha;
	void CPPPaintStart(void *BitmapData,int Width,int Height);
	void CPPPaintFinish(void *BitmapData,int Width,int Height);
	
	void SetupBitmap(HDC DC);
	void CPPPaint(cDNetGDIThreadCPPContext *CPP);
};
//---------------------------------------------------------------------------
class cD3D9ExWindowDevice : public iReference, public iWindowMessageHandler
{
public:
	cD3D9ExWindowDevice(iWindow *Window);
	~cD3D9ExWindowDevice();

	IDirect3DDevice9Ex* GetDevice(void)const;

	virtual void cnLib_FUNC WindowAttached(void)override;
	virtual void cnLib_FUNC WindowDetached(void)override;
	virtual bool cnLib_FUNC WindowMessage(LRESULT &Result,const cWindowMessageParam &MsgParam)override;
	virtual void cnLib_FUNC WindowMessageProcessed(LRESULT Result,const cWindowMessageParam &MsgParam)override;

protected:

	iWindow *fWindow;
	COMPtr<IDirect3DDevice9Ex> fDevice;
	static const D3DPRESENT_PARAMETERS DefaultPresentParameter;
};
//---------------------------------------------------------------------------
class cWPFDXGIViewContent : public iDXGIViewContent, public bcWPFViewContentDrawing
{
public:
	cWPFDXGIViewContent(iDXGIPainter *Painter);
	~cWPFDXGIViewContent();

	virtual cGCRef* GetDrawingPointer(void)override;

	// iUIViewContent

	virtual iUIView* cnLib_FUNC GetView(void)override;
	virtual bool cnLib_FUNC SetView(iUIView *View)override;
	virtual bool cnLib_FUNC GetVisible(void)override;
	virtual bool cnLib_FUNC SetVisible(bool Visible)override;
	virtual Float32 cnLib_FUNC GetZPosition(void)override;
	virtual bool cnLib_FUNC SetZPosition(Float32 ZPosition)override;
	virtual Float32 cnLib_FUNC GetContentScale(void)override;

	// iUIDCViewContent

	virtual eUIState cnLib_FUNC GetPaintState(void)override;
	virtual cUIPoint cnLib_FUNC GetPaintSize(void)override;
	virtual bool GetRenderBufferSharedHandle(HANDLE &SharedHandle)override;
	virtual void UpdateRenderBuffer(void)override;

protected:
private:

	mcWPFD3DImageDrawing fDrawing;

	iDXGIPainter *fPainter;
	iWindow *fWindow=nullptr;
	int fDrawWidth;
	int fDrawHeight;

	virtual void ViewContentUpdateRect(void)override;
	virtual void ViewContentUpdateState(void)override;
	virtual void ViewContentUpdateWindow(iWindow *Window)override;

	enum ePainterState
	{
		psNone,
		psBackground,
		psActive
	};
	ePainterState fPainterState;
	ePainterState CheckPainterState(void);
	static void NotifyPainterState(iDXGIPainter *Painter,ePainterState NewState,ePainterState OldState);
	void UpdatePainterState(void);

	template<class T>
	class cAffixedVariable
	{
	public:
		template<class THost>
		void Affix(const THost &Host)noexcept(true){
			fVariable=Host->QueryAffixedVariable(cnVar::TTypeID<T>::Value);
			fValue=static_cast<T*>(fVariable->Get());
			if(fValue==nullptr){
				auto NewContent=rCreate<T>(Host);
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

	void FrontAvailableChanged(void);
	class cFrontAvailableChangedProcedure : public iProcedure
	{
		virtual void cnLib_FUNC Execute(void)override;
	}fFrontAvailableChangedProcedure;

	void SetupBackBuffer(void);
	void ClearBackBuffer(void);
};
//---------------------------------------------------------------------------
}	// namespace cnWin
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
