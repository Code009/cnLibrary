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
