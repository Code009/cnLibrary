/*- cnUI - Grid Container -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-10-25                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_cnUI_GridContainer_H__
#define __cnLib_cnUI_GridContainer_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#include <cnUI/ScrollContainer.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
struct cGridItemLine
{
	cUIRange Range;
	cArray<cUIRange> Parts;
};
//---------------------------------------------------------------------------
struct cGridData
{
	cArray<const cGridItemLine> VerticalLines;
	cArray<const cGridItemLine> HorizontalLines;
};
//---------------------------------------------------------------------------
class scItemGrid : private iScrollContent, private cUIVisualDataCache<cGridData>::iDataProvider
{
public:
	scItemGrid()noexcept(true);
	~scItemGrid()noexcept(true);

	cCallbackSet ScrollContentUpdated;

	iScrollContentContainer* GetContainer(void)noexcept(true);
	void SetContainer(iScrollContentContainer *Container)noexcept(true);

	iUIView* GetView(void)const noexcept(true);
	void UpdateColumnCount(uIntn ColumnCount,bool IsEstimated)noexcept(true);
	void SetDefaultColumnSize(ufInt16 Size)noexcept(true);
	void SetDefaultColumnPadding(ufInt16 Size)noexcept(true);
	void UpdateRowCount(uIntn RowCount,bool IsEstimated)noexcept(true);
	void SetDefaultRowSize(ufInt16 Size)noexcept(true);
	void SetDefaultRowPadding(ufInt16 Size)noexcept(true);

	sfInt16 GetRowSize(uIntn RowIndex)noexcept(true);
	void SetRowSize(uIntn RowIndex,sfInt16 Size)noexcept(true);
	void ResetRowSize(uIntn RowIndex)noexcept(true);

	sfInt16 GetRowHeadPadding(uIntn RowIndex)noexcept(true);
	void SetRowHeadPadding(uIntn RowIndex,sfInt16 Size)noexcept(true);
	void ResetRowHeadPadding(uIntn RowIndex)noexcept(true);
	sfInt16 GetRowTailPadding(uIntn RowIndex)noexcept(true);
	void SetRowTailPadding(uIntn RowIndex,sfInt16 Size)noexcept(true);
	void ResetRowTailPadding(uIntn RowIndex)noexcept(true);

	sfInt16 GetColumnSize(uIntn ColumnIndex)noexcept(true);
	void SetColumnSize(uIntn ColumnIndex,sfInt16 Size)noexcept(true);
	void ResetColumnSize(uIntn ColumnIndex)noexcept(true);

	sfInt16 GetColumnHeadPadding(uIntn ColumnIndex)noexcept(true);
	void SetColumnHeadPadding(uIntn ColumnIndex,sfInt16 Size)noexcept(true);
	void ResetColumnHeadPadding(uIntn ColumnIndex)noexcept(true);
	sfInt16 GetColumnTailPadding(uIntn ColumnIndex)noexcept(true);
	void SetColumnTailPadding(uIntn ColumnIndex,sfInt16 Size)noexcept(true);
	void ResetColumnTailPadding(uIntn ColumnIndex)noexcept(true);

	Float32 GetRowOffset(void)const noexcept(true);
	Float32 GetColumnOffset(void)const noexcept(true);
	bool SetGridOffset(Float32 RowOffset,Float32 ColumnOffset)noexcept(true);

	enum class ItemLayer
	{
		Normal,
		Leave,
		Stay,
	};
protected:

	class bcItem : public iScrollContentContainer
	{
	public:
		virtual iUIView* GetScrollView(void)noexcept(true)override;
		virtual iScrollContent* GetContent(void)noexcept(true)override;
		virtual void SetContent(iScrollContent *ScrollItem)noexcept(true)override;
		virtual cUIPoint GetOffset(void)noexcept(true)override;
		virtual bool SetOffset(cUIPoint Offset)noexcept(true)override;
		virtual cUIRange GetScrollLimitX(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)override;
		virtual void SetScrollLimitX(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)override;
		virtual cUIRange GetScrollLimitY(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)override;
		virtual void SetScrollLimitY(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)override;
		virtual cUIPoint GetViewportSize(void)noexcept(true)override;
		virtual void NotifyUpdateScroll(void)noexcept(true)override;
		virtual bool IsRectangleInViewport(const cUIRectangle &Rect)noexcept(true)override;

		ItemLayer GetLayer(void)const noexcept(true);
	protected:
		friend scItemGrid;
		scItemGrid *fOwner;
		iScrollContent *fContent=nullptr;
		//cUIRectangle fContentRect;
		bool fVisible;
		bool fNotifiedVisible;

		bool fStay;
		bool fStayLeaving;

		void UpdateVisible(void)noexcept(true);
		void NotifyContentUpdate(void)noexcept(true);
	};


public:

	class cRow : public bcItem, private cnRTL::cRTLAllocator
	{
	public:
		cRow()noexcept(true);
		~cRow()noexcept(true);

		bool Close(void)noexcept(true);
		uIntn GetRowIndex(void)const noexcept(true);
		bool SetRowIndex(uIntn NewRowIndex)noexcept(true);

		void SetStayRange(uIntn Begin,uIntn End)noexcept(true);
		void ResetStayRange(void)noexcept(true);

		cUIRect CalculateLayoutRect(void)const noexcept(true);

	private:
		friend scItemGrid;
		uIntn fRowIndex;
		uIntn fStayBeginIndex;
		uIntn fStayEndIndex;
		cUIRange fStayPos;
	};
	class cColumn : public bcItem, private cnRTL::cRTLAllocator
	{
	public:
		cColumn()noexcept(true);
		~cColumn()noexcept(true);

		bool Close(void)noexcept(true);
		uIntn GetColumnIndex(void)const noexcept(true);
		bool SetColumnIndex(uIntn NewColumnIndex)noexcept(true);
	
		cUIRect CalculateLayoutRect(void)const noexcept(true);
	private:
		friend scItemGrid;
		uIntn fColumnIndex;
		uIntn fStayBeginIndex;
		uIntn fStayEndIndex;
		cUIRange fStayPos;
	};
	class cCell : public bcItem, private cnRTL::cRTLAllocator
	{
	public:
		cCell()noexcept(true);
		~cCell()noexcept(true);

		bool Close(void)noexcept(true);
		uIntn GetRowIndex(void)const noexcept(true);
		uIntn GetColumnIndex(void)const noexcept(true);
		bool Move(uIntn NewRowIndex,uIntn NewColumnIndex)noexcept(true);

		cUIRect CalculateLayoutRect(void)const noexcept(true);
	private:
		friend scItemGrid;
		uIntn fRowIndex;
		uIntn fColumnIndex;
		cUIRectangle fStayRect;
	};
	cRow* FindRow(uIntn RowIndex)noexcept(true);
	cRow* QueryRow(uIntn RowIndex)noexcept(true);
	bool CloseRow(uIntn RowIndex)noexcept(true);
	cRow* MoveRow(uIntn RowIndex,uIntn NewRowIndex)noexcept(true);


	cColumn* FindColumn(uIntn ColumnIndex)noexcept(true);
	cColumn* QueryColumn(uIntn ColumnIndex)noexcept(true);
	bool CloseColumn(uIntn ColumnIndex)noexcept(true);
	cColumn* MoveColumn(uIntn ColumnIndex,uIntn NewColumnIndex)noexcept(true);

	cCell* FindCell(uIntn RowIndex,uIntn ColumnIndex)noexcept(true);
	cCell* QueryCell(uIntn RowIndex,uIntn ColumnIndex)noexcept(true);
	bool CloseCell(uIntn RowIndex,uIntn ColumnIndex)noexcept(true);
	cCell* MoveCell(uIntn RowIndex,uIntn ColumnIndex,uIntn NewRowIndex,uIntn NewColumnIndex)noexcept(true);

	operator iVisualData<cGridData> *()const noexcept(true);

	uIntn GetVisibleRowBegin(void)noexcept(true);
	uIntn GetVisibleRowEnd(void)noexcept(true);
	uIntn GetVisibleColBegin(void)noexcept(true);
	uIntn GetVisibleColEnd(void)noexcept(true);

	uIntn FindRowIndexAt(Float32 RowPosition)noexcept(true);
	uIntn FindColumnIndexAt(Float32 ColumnPosition)noexcept(true);

	bool FindRowIndexAt(Float32 RowPosition,uIntn &RowIndex)noexcept(true);
	bool FindColumnIndexAt(Float32 ColumnPosition,uIntn &ColumnIndex)noexcept(true);

	bool IsRowVisible(uIntn RowIndex)const noexcept(true);
	bool IsColumnVisible(uIntn ColumnIndex)const noexcept(true);
protected:

	iScrollContentContainer *fContainer=0;

	virtual void ScrollContentShow(void)noexcept(true)override;
	virtual void ScrollContentHide(void)noexcept(true)override;
	virtual void ScrollContentUpdate(void)noexcept(true)override;

	cUIRange CalculateRowRange(uIntn RowIndex)noexcept(true);
	cUIRange CalculateColumnRange(uIntn ColumnIndex)noexcept(true);

private:

	uIntn fVisibleRowIndex;
	uIntn fVisibleColumnIndex;


	cnRTL::cSeqMap<uIntn,cRow*> fRowMap;
	cnRTL::cSeqMap<uIntn,cColumn*> fColMap;

	struct cGridIndex
	{
		uIntn Row;
		uIntn Column;

		sfInt8 Compare(const cGridIndex &Src)const noexcept(true);

		bool operator ==(const cGridIndex &Src)const noexcept(true){	return Row==Src.Row && Column==Src.Column;	}
		bool operator !=(const cGridIndex &Src)const noexcept(true){	return Row!=Src.Row || Column!=Src.Column;	}
		cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(,(const cGridIndex &Src)const,(Compare(Src)))
	};
	cnRTL::cSeqMap<cGridIndex,cCell*> fCellMap;

	struct cPositionRecord
	{
		sfInt16 Size;
		sfInt16 HeadPadding;
		sfInt16 TailPadding;
		bool Update;
		Float32 LastRecordEnd;
		Float32 Begin;
		Float32 End;
	};
	cnRTL::cSeqMap<uIntn,cPositionRecord> fRowRangeMap;
	cnRTL::cSeqMap<uIntn,cPositionRecord> fColRangeMap;


	cnRTL::cSeqList<cUIRange> fVisibleRowRanges;
	cnRTL::cSeqList<cUIRange> fVisibleColRanges;
	cnRTL::cSeqList<bcItem*> fVisibleItemList;
	cnRTL::cSeqList<bcItem*> fVisibleItemTempList;

	template<class TItem>
	struct cStayCalculateState
	{
		TItem *Item;
		cRow *Row;
		Float32 Limit;
		Float32 Size;
	};
	cnRTL::cSeqList<cRow*> fRowStayList;
	cnRTL::cSeqList<cColumn*> fColStayList;
	cnRTL::cSeqList<cCell*> fCellStayList;


	struct cCompareRange
	{
		Float32 Position;
		sfInt8 operator ()(const cPositionRecord &Src)noexcept(true);
	};

	static cnDataStruct::cSeqMapIterator<uIntn,cPositionRecord> FindRangeLowwer(Float32 RowPosition,bool &Match,cnRTL::cSeqMap<uIntn,cPositionRecord> &RangeMap)noexcept(true);
	static uIntn ScanVisibleRange(cUIRange VisibleRange,sfInt16 DefaultSize,sfInt16 DefaultPadding,cnRTL::cSeqMap<uIntn,cPositionRecord> &RangeMap,cnRTL::cSeqList<cUIRange> &VisibleRangeList)noexcept(true);
	template<class TItem>
	static void ScanStayItems(cUIRange VisibleRange,sfInt16 DefaultSize,sfInt16 DefaultPadding,uIntn VisibleIndex,uIntn VisibleEndIndex,const cnRTL::cSeqList<cUIRange> &VisibleRanges,const cnRTL::cSeqMap<uIntn,TItem*> &ItemMap,const cnRTL::cSeqMap<uIntn,cPositionRecord> &ItemRangeMap,cnRTL::cSeqList<TItem*> &StayList)noexcept(true);


	uIntn fRowCount;
	uIntn fColumnCount;
	ufInt16 fDefaultRowSize=0;
	ufInt16 fDefaultRowPadding=0;
	ufInt16 fDefaultColumnSize=0;
	ufInt16 fDefaultColumnPadding=0;


	class cDataReference : public iReference
	{
	public:
		cGridData Data;
		cnRTL::cSeqList<cTextTabItem> ItemList;
	};
	rPtr< cUIVisualDataCache<cGridData> > fDataCache;
	virtual const cGridData* QueryData(rPtr<iReference> &DataReference)noexcept(true)override;
};
//---------------------------------------------------------------------------
class blItemGridManager : public LayoutControl
{
public:
	blItemGridManager()noexcept(true);
	~blItemGridManager()noexcept(true);


	scItemGrid* GetGrid(scItemGrid *Grid)noexcept(true);
	void SetGrid(scItemGrid *Grid)noexcept(true);

protected:
	scItemGrid *fGrid;
	virtual void GridSetup(void)noexcept(true);
	virtual void GridClear(void)noexcept(true);
	virtual void GridUpdate(uIntn RowIndex,uIntn RowEndIndex,uIntn ColIndex,uIntn ColEndIndex)noexcept(true)=0;

private:
	iFunctionToken *fScrollContentUpdateToken;

	void GridUpdateData(void)noexcept(true);
};
//---------------------------------------------------------------------------
class bcItemGridRow : public Form, public iScrollContent
{
public:
	scItemGrid::cRow *Row=nullptr;

	virtual void ScrollContentShow(void)noexcept(true)override;
	virtual void ScrollContentHide(void)noexcept(true)override;
	virtual void ScrollContentUpdate(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class bcItemGridColumn : public Form, public iScrollContent
{
public:
	scItemGrid::cColumn *Column=nullptr;

	virtual void ScrollContentShow(void)noexcept(true)override;
	virtual void ScrollContentHide(void)noexcept(true)override;
	virtual void ScrollContentUpdate(void)noexcept(true)override;

};
//---------------------------------------------------------------------------
class bcItemGridCell : public Form, public iScrollContent
{
public:
	scItemGrid::cCell *Cell=nullptr;

	virtual void ScrollContentShow(void)noexcept(true)override;
	virtual void ScrollContentHide(void)noexcept(true)override;
	virtual void ScrollContentUpdate(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
template<class TGridRow>
class cItemGridRowSet
{
public:
	~cItemGridRowSet()noexcept(true){
		for(auto RowControl : fRowControlSet){
			if(RowControl->Row!=nullptr){
				RowControl->Row->Close();
			}
			delete RowControl;
		}
	}

	void ViewSetup(iUIView *ParentView)noexcept(true){
		for(auto *RowControl : fRowControlSet){
			ParentView->InsertView(*RowControl);
		}
	}
	void ViewClear(iUIView *ParentView)noexcept(true){
		for(auto *RowControl : fRowControlSet){
			ParentView->RemoveView(*RowControl);
		}
	}

	void Layout(iUIView *ParentView)noexcept(true){
		for(auto *RowControl : fRowControlSet){
			auto LayoutRect=RowControl->Row->CalculateLayoutRect();
			ControlSetRect(*RowControl,ParentView,LayoutRect);
		}
	}

	TGridRow* Query(iUIView *ParentView,scItemGrid *Grid,uIntn RowIndex)noexcept(true){
		auto Row=Grid->QueryRow(RowIndex);

		TGridRow *RowControl;
		auto RowContent=Row->GetContent();
		if(RowContent!=nullptr){
			RowControl=static_cast<TGridRow*>(RowContent);
		}
		else{
			// recycle invisible control

			RowControl=new TGridRow;
			RowControl->Row=Row;
			fRowControlSet.Insert(RowControl);
			Row->SetContent(RowControl);
			if(ParentView!=nullptr){
				ParentView->InsertView(*RowControl);
			}
		}

		return RowControl;
	}
protected:
private:
	cnRTL::cSeqSet<TGridRow*> fRowControlSet;
};
//---------------------------------------------------------------------------
template<class TGridColumn>
class cItemGridColumnSet
{
public:
	~cItemGridColumnSet()noexcept(true){
		for(auto ColControl : fColumnControlSet){
			if(ColControl->Col!=nullptr){
				ColControl->Col->Close();
			}
			delete ColControl;
		}
	}

	void ViewSetup(iUIView *ParentView)noexcept(true){
		for(auto *ColControl : fColumnControlSet){
			ParentView->InsertView(*ColControl);
		}
	}
	void ViewClear(iUIView *ParentView)noexcept(true){
		for(auto *ColControl : fColumnControlSet){
			ParentView->RemoveView(*ColControl);
		}
	}

	void Layout(iUIView *ParentView)noexcept(true){
		for(auto *ColControl : fColumnControlSet){
			auto LayoutRect=ColControl->Cell->CalculateLayoutRect();
			ControlSetRect(*ColControl,ParentView,LayoutRect);
		}
	}

	TGridColumn* Query(iUIView *ParentView,scItemGrid *Grid,uIntn ColIndex)noexcept(true){
		auto Column=Grid->QueryColumn(ColIndex);

		TGridColumn *ColControl;
		auto ColContent=Column->GetContent();
		if(ColContent!=nullptr){
			ColControl=static_cast<TGridColumn*>(ColContent);
		}
		else{
			// recycle invisible control

			ColControl=new TGridColumn;
			ColControl->Column=Column;
			fColumnControlSet.Insert(ColControl);
			Column->SetContent(ColControl);
			if(ParentView!=nullptr){
				ParentView->InsertView(*ColControl);
			}
		}

		return ColControl;
	}
protected:
private:
	cnRTL::cSeqSet<TGridColumn*> fColumnControlSet;
};
//---------------------------------------------------------------------------
template<class TGridCell>
class cItemGridCellSet
{
public:
	~cItemGridCellSet()noexcept(true){
		for(auto CellControl : fCellControlSet){
			if(CellControl->Cell!=nullptr){
				CellControl->Cell->Close();
			}
			delete CellControl;
		}
	}

	void ViewSetup(iUIView *ParentView)noexcept(true){
		for(auto *CellControl : fCellControlSet){
			ParentView->InsertView(*CellControl);
		}
	}
	void ViewClear(iUIView *ParentView)noexcept(true){
		for(auto *CellControl : fCellControlSet){
			ParentView->RemoveView(*CellControl);
		}
	}

	void Layout(iUIView *ParentView)noexcept(true){
		for(auto *CellControl : fCellControlSet){
			auto LayoutRect=CellControl->Cell->CalculateLayoutRect();
			ControlSetRect(*CellControl,ParentView,LayoutRect);
		}
	}

	TGridCell* Query(iUIView *ParentView,scItemGrid *Grid,uIntn RowIndex,uIntn ColIndex)noexcept(true){
		auto Cell=Grid->QueryCell(RowIndex,ColIndex);

		TGridCell *CellControl;
		auto CellContent=Cell->GetContent();
		if(CellContent!=nullptr){
			CellControl=static_cast<TGridCell*>(CellContent);
		}
		else{
			// recycle invisible control

			CellControl=new TGridCell;
			CellControl->Cell=Cell;
			fCellControlSet.Insert(CellControl);
			Cell->SetContent(CellControl);
			if(ParentView!=nullptr){
				ParentView->InsertView(*CellControl);
			}
		}

		return CellControl;
	}
protected:
private:
	cnRTL::cSeqSet<TGridCell*> fCellControlSet;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE viGridLineData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> GridLineNotifySet;
	struct cGridLineInfo
	{
		ufInt16 HorizontalItemCount;
		ufInt16 VerticalItemCount;
	};
	virtual cGridLineInfo GridLineInfo(void)noexcept(true)=0;
	virtual cUIRange GridLineRange(bool Vertical,ufInt16 Index)noexcept(true)=0;
	virtual ufInt16 GridLinePartCount(bool Vertical,ufInt16 Index)noexcept(true)=0;
	virtual cUIRange GridLinePartRange(bool Vertical,ufInt16 Index,ufInt16 PartIndex)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cGridLineData : public viGridLineData
{
public:

	virtual cGridLineInfo GridLineInfo(void)noexcept(true)override;
	virtual cUIRange GridLineRange(bool Vertical,ufInt16 Index)noexcept(true)override;
	virtual ufInt16 GridLinePartCount(bool Vertical,ufInt16 Index)noexcept(true)override;
	virtual cUIRange GridLinePartRange(bool Vertical,ufInt16 Index,ufInt16 PartIndex)noexcept(true)override;

protected:

	void GridLineSetupStart(ufInt16 HorizontalCount,ufInt16 VerticalCount,cUIRange HoriRange,cUIRange VertRange)noexcept(true);
	void GridLineSetupFinish(void)noexcept(true);
	void GridLineSetupRange(bool Vertical,ufInt16 Index,const cUIRange &Range)noexcept(true);
	void GridLineSetupGap(bool Vertical,ufInt16 Index,ufInt16 GapIndex,ufInt16 GapEndIndex)noexcept(true);


private:

	struct cLineItem
	{
		cUIRange Range;
		cnRTL::cSeqList<bool> Gaps;
		cnRTL::cSeqList<cUIRange> Parts;
	};
	cnRTL::cSeqList<cLineItem> fLines[2];
	cUIRange fRanges[2];

	void Finish(bool Vertical)noexcept(true);
};
//---------------------------------------------------------------------------
class bcViewList : public cScrollViewContainer, public cGridLineData
{
public:
	bcViewList()noexcept(true);
	~bcViewList()noexcept(true);

	bool GetHorizontalList(void)const noexcept(true);
	void SetHorizontalList(bool Value)noexcept(true);

	Float32 GetListOffset(void)const noexcept(true);
	void SetListOffset(Float32 ListOffset)noexcept(true);

	class cItem : public bcViewItem
	{
	public:
		cItem()noexcept(true);
		~cItem()noexcept(true);

		iViewReference* GetViewReference(void)const noexcept(true);
		void SetViewReference(iViewReference *ViewReference)noexcept(true);

	private:
		friend bcViewList;
		bcViewList *fOwner;
		ufInt32 fItemIndex;

		void SetupOwner(bcViewList *Owner,ufInt32 ItemIndex)noexcept(true);
		void ClearOwner(void)noexcept(true);
	};
	aClsRef<cItem> QueryItem(ufInt32 ItemIndex)noexcept(true);


	//cUIRange QueryItemRange(ufInt32 ItemIndex)noexcept(true);

	ufInt32 FindItemIndexAt(Float32 Position)noexcept(true);

	bool FindItemIndexAt(Float32 Position,ufInt32 &ItemIndex)noexcept(true);

	bool IsItemVisible(ufInt32 ItemIndex)const noexcept(true);
protected:

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	// control list

	//			Item Direction
	// true	:	x
	// false:	y
	bool fListXItem;

	// Position - DisplayIndex

	virtual cUIRange ViewListDisplayQueryRange(ufInt32 DisplayIndex)noexcept(true)=0;
	virtual bool ViewListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)noexcept(true)=0;
	virtual void ViewListDisplaySetupItems(ufInt32 DisplayIndex,ufInt32 DisplayEndIndex)noexcept(true)=0;
	
	// DisplayIndex - ItemIndex

	virtual ufInt32 ViewListItemIndexToDisplayIndex(ufInt32 ItemIndex)noexcept(true);
	virtual ufInt32 ViewListDisplayIndexToItemIndex(ufInt32 DisplayIndex)noexcept(true);

	void SetNeedUpdateListViews(void)noexcept(true);
	void SetNeedUpdateListLayout(void)noexcept(true);

private:
	bool fInLayout;
	bool fNeedUpdateListViews;
	bool fNeedUpdateListLayout;

	ufInt32 fViewportDisplayIndex;
	ufInt32 fViewportDisplayEndIndex;

	cnRTL::cSeqMap<ufInt32,aCls<cItem>*> fItemMap;

	cnRTL::cSeqSet< aClsRef<cItem> > fInUseItemSet;

	void ItemDeleted(cItem *Item)noexcept(true);

	void PositItem(cItem *Item)noexcept(true);
	
};
typedef bcViewList::cItem cViewListItem;
//---------------------------------------------------------------------------
class bcViewGrid : public cScrollViewContainer, public cGridLineData
{
public:
	bcViewGrid()noexcept(true);
	~bcViewGrid()noexcept(true);

	bool GetVerticalColumn(void)const noexcept(true);
	void SetVerticalColumn(bool Value)noexcept(true);
	Float32 GetRowOffset(void)const noexcept(true);
	Float32 GetColumnOffset(void)const noexcept(true);

	void SetGridOffset(Float32 RowOffset,Float32 ColumnOffset)noexcept(true);

	class cRow;
	class cCell : public bcViewItem
	{
	public:
		cCell()noexcept(true);
		~cCell()noexcept(true);
		iViewReference* GetViewReference(void)const noexcept(true);
		void SetViewReference(iViewReference *ViewReference)noexcept(true);
	private:
		friend bcViewGrid;
		friend cRow;
		cRow *fOwner;
		ufInt32 fColumnIndex;

		void SetupOwner(cRow *Owner,ufInt32 ColumnIndex)noexcept(true);
		void ClearOwner(void)noexcept(true);
	};
	class cRow : public bcViewItem
	{
	public:
		cRow()noexcept(true);
		~cRow()noexcept(true);
		iViewReference* GetViewReference(void)const noexcept(true);
		void SetViewReference(iViewReference *ViewReference)noexcept(true);
		aClsRef<cCell> QueryCell(ufInt32 ColumnIndex)noexcept(true);
	private:
		friend bcViewGrid;
		bcViewGrid *fOwner;
		ufInt32 fRowIndex;
		cnRTL::cSeqMap<ufInt32,aCls<cCell>*> fCellMap;

		void CellDeleted(cCell *Cell)noexcept(true);

		void SetupOwner(bcViewGrid *Owner,ufInt32 RowIndex)noexcept(true);
		void ClearOwner(void)noexcept(true);
	};
	aClsRef<cRow> QueryRow(ufInt32 RowIndex)noexcept(true);


	//cUIRange QueryRowRange(ufInt32 RowIndex)noexcept(true);
	//cUIRange QueryColumnRange(ufInt32 ColumnIndex)noexcept(true);

	ufInt32 FindRowIndexAt(Float32 RowPosition)noexcept(true);
	ufInt32 FindColumnIndexAt(Float32 ColumnPosition)noexcept(true);

	bool FindRowIndexAt(Float32 RowPosition,ufInt32 &RowIndex)noexcept(true);
	bool FindColumnIndexAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)noexcept(true);

	bool IsRowVisible(ufInt32 RowIndex)const noexcept(true);
protected:

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	// control list

	union{
		//			row		column
		// true	:	x		y
		// false:	y		x
		
		bool fGridXRow;
		bool fGridYColumn;
	};


	virtual cUIRange ViewGridDisplayRowQueryRange(ufInt32 DisplayRowIndex)noexcept(true)=0;
	virtual cUIRange ViewGridDisplayColumnQueryRange(ufInt32 DisplayColumnIndex)noexcept(true)=0;
	virtual bool ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &DisplayRowIndex)noexcept(true)=0;
	virtual bool ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &DisplayColumnIndex)noexcept(true)=0;
	virtual void ViewGridDisplaySetupRows(bool UpdateRow,ufInt32 DisplayRowIndex,ufInt32 DisplayRowEndIndex,ufInt32 DisplayColIndex,ufInt32 DisplayColEndIndex)noexcept(true)=0;
	
	virtual ufInt32 ViewGridDisplayRowIndexToRowIndex(ufInt32 DisplayRowIndex)noexcept(true);
	virtual ufInt32 ViewGridRowIndexToDisplayRowIndex(ufInt32 RowIndex)noexcept(true);
	virtual ufInt32 ViewGridDisplayColumnIndexToColumnIndex(ufInt32 DisplayColumnIndex)noexcept(true);
	virtual ufInt32 ViewGridColumnIndexToDisplayColumnIndex(ufInt32 ColumnIndex)noexcept(true);

	void SetNeedUpdateGridViews(void)noexcept(true);
	void SetNeedUpdateGridLayout(void)noexcept(true);

private:
	bool fInLayout;
	bool fNeedUpdateGridLayout;
	bool fNeedUpdateGridViews;

	ufInt32 fViewportDisplayColumnIndex;
	ufInt32 fViewportDisplayColumnEndIndex;

	ufInt32 fViewportDisplayRowIndex;
	ufInt32 fViewportDisplayRowEndIndex;

	cnRTL::cSeqMap<ufInt32,aCls<cRow>*> fRowMap;

	cnRTL::cSeqSet< aClsRef<cRow> > fInUseRowSet;
	cnRTL::cSeqSet< aClsRef<cCell> > fInUseCellSet;

	void RowDeleted(cRow *Row)noexcept(true);

	void PositRow(cRow *Row)noexcept(true);
	void PositCell(cCell *Cell)noexcept(true);
	
};

typedef bcViewGrid::cRow cViewGridRow;
typedef bcViewGrid::cCell cViewGridCell;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlListSource
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlListNotifySet;

	virtual bool ListGetItemCount(ufInt32 &ItemCount)noexcept(true)=0;
	virtual void ListSetupItems(ufInt32 RowIndex,ufInt32 RowCount)noexcept(true)=0;
	virtual void ListClearItems(void)noexcept(true)=0;
	virtual Float32 ListItemGetSize(ufInt32 RowIndex)noexcept(true)=0;
	virtual Float32 ListItemGetDistance(ufInt32 RowIndex)noexcept(true)=0;
	virtual rPtr<iViewReference> ListItemMake(cScrollViewContainer *Container,ufInt32 ItemIndex)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cControlList : public bcViewList
{
public:
	cControlList()noexcept(true);
	~cControlList()noexcept(true);

	iControlListSource* GetControlSource(void)const noexcept(true);
	void SetControlSource(iControlListSource *Source)noexcept(true);


	Float32 ItemSize=0;
	Float32 ItemBorder=0;
protected:
	iControlListSource *fControlSource=nullptr;


	// bcViewList

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;
	
	virtual cUIRange ViewListDisplayQueryRange(ufInt32 DisplayIndex)noexcept(true)override;
	virtual bool ViewListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)noexcept(true)override;
	virtual void ViewListDisplaySetupItems(ufInt32 DisplayIndex,ufInt32 DisplayEndIndex)noexcept(true)override;
	
	// DisplayIndex - ItemIndex

private:

	iFunctionToken *fListNotifyToken;
	void ApplyControlSource(void)noexcept(true);
	ufInt32 fListItemCount;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlGridSource
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlGridNotifySet;

	virtual ufInt16 GridColumnCount(void)noexcept(true)=0;
	virtual Float32 GridColumnGetSize(ufInt16 Index)noexcept(true)=0;
	virtual Float32 GridColumnGetDistance(ufInt16 Index)noexcept(true)=0;

	virtual bool GridGetRowCount(ufInt32 &RowCount)noexcept(true)=0;
	virtual void GridSetupRows(ufInt32 RowIndex,ufInt32 RowCount)noexcept(true)=0;
	virtual void GridClearRows(void)noexcept(true)=0;
	virtual Float32 GridRowGetSize(ufInt32 RowIndex)noexcept(true)=0;
	virtual Float32 GridRowGetDistance(ufInt32 RowIndex)noexcept(true)=0;
	virtual rPtr<iViewReference> GridRowMakeCell(cScrollViewContainer *Container,ufInt32 RowIndex,ufInt32 ColIndex)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cControlGrid : public bcViewGrid
{
public:
	cControlGrid()noexcept(true);
	~cControlGrid()noexcept(true);

	iControlGridSource* GetControlSource(void)const noexcept(true);
	void SetControlSource(iControlGridSource *Source)noexcept(true);


	Float32 RowSize=0;
	Float32 RowBorder=0;
protected:
	iControlGridSource *fControlSource;


	// bcViewGrid

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;
	
	virtual cUIRange ViewGridDisplayRowQueryRange(ufInt32 RowIndex)noexcept(true)override;
	virtual cUIRange ViewGridDisplayColumnQueryRange(ufInt32 ColumnIndex)noexcept(true)override;
	virtual bool ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &RowIndex)noexcept(true)override;
	virtual bool ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)noexcept(true)override;
	virtual void ViewGridDisplaySetupRows(bool UpdateRow,ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)noexcept(true)override;

private:
	iFunctionToken *fGridNotifyToken;

	void ApplyControlSource(void)noexcept(true);
	struct cColumnItem
	{
		cUIRange Range;
	};
	cnRTL::cSeqList<cColumnItem> fGridColumns;
	ufInt32 fGridRowCount;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cControlGridList : public bcViewGrid
{
public:
	cControlGridList()noexcept(true);
	~cControlGridList()noexcept(true);

	Float32 ColumnSize=40;
	Float32 ColumnBorder=0;
	Float32 RowSize=40;
	Float32 RowBorder=1;


	iControlListSource* GetControlSource(void)const noexcept(true);
	void SetControlSource(iControlListSource *Source)noexcept(true);


	ufInt16 GetColumnCount(void)const noexcept(true);

protected:
	iControlListSource *fControlSource;
	
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)noexcept(true)override;

	// bcViewGrid
	
	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;
	
	virtual cUIRange ViewGridDisplayRowQueryRange(ufInt32 RowIndex)noexcept(true)override;
	virtual cUIRange ViewGridDisplayColumnQueryRange(ufInt32 ColumnIndex)noexcept(true)override;
	virtual bool ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &RowIndex)noexcept(true)override;
	virtual bool ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)noexcept(true)override;
	virtual void ViewGridDisplaySetupRows(bool UpdateRow,ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)noexcept(true)override;

private:
	
	iFunctionToken *fListNotifyToken;
	ufInt16 fColumnCount;

	void ApplyControlSource(void)noexcept(true);
	ufInt32 fListItemCount;

};
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
//---------------------------------------------------------------------------
