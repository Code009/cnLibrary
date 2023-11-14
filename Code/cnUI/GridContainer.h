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
	cFunctionSet<void (void)> GridLineNotifySet;
	struct cGridLineInfo
	{
		ufInt16 HorizontalItemCount;
		ufInt16 VerticalItemCount;
	};
	virtual cGridLineInfo GridLineInfo(void)=0;
	virtual cUIRange GridLineRange(bool Vertical,ufInt16 Index)=0;
	virtual ufInt16 GridLinePartCount(bool Vertical,ufInt16 Index)=0;
	virtual cUIRange GridLinePartRange(bool Vertical,ufInt16 Index,ufInt16 PartIndex)=0;
};
//---------------------------------------------------------------------------
class cGridLineData : public viGridLineData
{
public:

	virtual cGridLineInfo GridLineInfo(void)override;
	virtual cUIRange GridLineRange(bool Vertical,ufInt16 Index)override;
	virtual ufInt16 GridLinePartCount(bool Vertical,ufInt16 Index)override;
	virtual cUIRange GridLinePartRange(bool Vertical,ufInt16 Index,ufInt16 PartIndex)override;

protected:

	void GridLineSetupStart(ufInt16 HorizontalCount,ufInt16 VerticalCount,cUIRange HoriRange,cUIRange VertRange);
	void GridLineSetupFinish(void);
	void GridLineSetupRange(bool Vertical,ufInt16 Index,const cUIRange &Range);
	void GridLineSetupGap(bool Vertical,ufInt16 Index,ufInt16 GapIndex,ufInt16 GapEndIndex);


private:

	struct cLineItem
	{
		cUIRange Range;
		cnRTL::cSeqList<bool> Gaps;
		cnRTL::cSeqList<cUIRange> Parts;
	};
	cnRTL::cSeqList<cLineItem> fLines[2];
	cUIRange fRanges[2];

	void Finish(bool Vertical);
};
//---------------------------------------------------------------------------
class bcViewList : public cScrollViewContainer, public cGridLineData
{
public:
	bcViewList();
	~bcViewList();

	bool GetHorizontalList(void)const;
	void SetHorizontalList(bool Value);

	Float32 GetListOffset(void)const;
	void SetListOffset(Float32 ListOffset);

	class cItem : public bcViewItem
	{
	public:
		cItem();
		~cItem();

		iViewReference* GetViewReference(void)const;
		void SetViewReference(iViewReference *ViewReference);

	private:
		friend bcViewList;
		bcViewList *fOwner;
		ufInt32 fItemIndex;

		void SetupOwner(bcViewList *Owner,ufInt32 ItemIndex);
		void ClearOwner(void);
	};
	aClsRef<cItem> QueryItem(ufInt32 ItemIndex);


	//cUIRange QueryItemRange(ufInt32 ItemIndex);

	ufInt32 FindItemIndexAt(Float32 Position);

	bool FindItemIndexAt(Float32 Position,ufInt32 &ItemIndex);

	bool IsItemVisible(ufInt32 ItemIndex)const;
protected:

	virtual void ScrollContentUpdateContentLayout(void)override;

	// control list

	//			Item Direction
	// true	:	x
	// false:	y
	bool fListXItem;

	// Position - DisplayIndex

	virtual cUIRange ViewListDisplayQueryRange(ufInt32 DisplayIndex)=0;
	virtual bool ViewListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)=0;
	virtual void ViewListDisplaySetupItems(ufInt32 DisplayIndex,ufInt32 DisplayEndIndex)=0;
	
	// DisplayIndex - ItemIndex

	virtual ufInt32 ViewListItemIndexToDisplayIndex(ufInt32 ItemIndex);
	virtual ufInt32 ViewListDisplayIndexToItemIndex(ufInt32 DisplayIndex);

	void SetNeedUpdateListViews(void);
	void SetNeedUpdateListLayout(void);

private:
	bool fInLayout;
	bool fNeedUpdateListViews;
	bool fNeedUpdateListLayout;

	ufInt32 fViewportDisplayIndex;
	ufInt32 fViewportDisplayEndIndex;

	cnRTL::cSeqMap<ufInt32,aCls<cItem>*> fItemMap;

	cnRTL::cSeqSet< aClsRef<cItem> > fInUseItemSet;

	void ItemDeleted(cItem *Item);

	void PositItem(cItem *Item);
	
};
typedef bcViewList::cItem cViewListItem;
//---------------------------------------------------------------------------
class bcViewGrid : public cScrollViewContainer, public cGridLineData
{
public:
	bcViewGrid();
	~bcViewGrid();

	bool GetVerticalColumn(void)const;
	void SetVerticalColumn(bool Value);
	Float32 GetRowOffset(void)const;
	Float32 GetColumnOffset(void)const;

	void SetGridOffset(Float32 RowOffset,Float32 ColumnOffset);

	class cRow;
	class cCell : public bcViewItem
	{
	public:
		cCell();
		~cCell();
		iViewReference* GetViewReference(void)const;
		void SetViewReference(iViewReference *ViewReference);
	private:
		friend bcViewGrid;
		friend cRow;
		cRow *fOwner;
		ufInt32 fColumnIndex;

		void SetupOwner(cRow *Owner,ufInt32 ColumnIndex);
		void ClearOwner(void);
	};
	class cRow : public bcViewItem
	{
	public:
		cRow();
		~cRow();
		iViewReference* GetViewReference(void)const;
		void SetViewReference(iViewReference *ViewReference);
		aClsRef<cCell> QueryCell(ufInt32 ColumnIndex);
	private:
		friend bcViewGrid;
		bcViewGrid *fOwner;
		ufInt32 fRowIndex;
		cnRTL::cSeqMap<ufInt32,aCls<cCell>*> fCellMap;

		void CellDeleted(cCell *Cell);

		void SetupOwner(bcViewGrid *Owner,ufInt32 RowIndex);
		void ClearOwner(void);
	};
	aClsRef<cRow> QueryRow(ufInt32 RowIndex);


	//cUIRange QueryRowRange(ufInt32 RowIndex);
	//cUIRange QueryColumnRange(ufInt32 ColumnIndex);

	ufInt32 FindRowIndexAt(Float32 RowPosition);
	ufInt32 FindColumnIndexAt(Float32 ColumnPosition);

	bool FindRowIndexAt(Float32 RowPosition,ufInt32 &RowIndex);
	bool FindColumnIndexAt(Float32 ColumnPosition,ufInt32 &ColumnIndex);

	bool IsRowVisible(ufInt32 RowIndex)const;
protected:

	virtual void ScrollContentUpdateContentLayout(void)override;

	// control list

	union{
		//			row		column
		// true	:	x		y
		// false:	y		x
		
		bool fGridXRow;
		bool fGridYColumn;
	};


	virtual cUIRange ViewGridDisplayRowQueryRange(ufInt32 DisplayRowIndex)=0;
	virtual cUIRange ViewGridDisplayColumnQueryRange(ufInt32 DisplayColumnIndex)=0;
	virtual bool ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &DisplayRowIndex)=0;
	virtual bool ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &DisplayColumnIndex)=0;
	virtual void ViewGridDisplaySetupRows(bool UpdateRow,ufInt32 DisplayRowIndex,ufInt32 DisplayRowEndIndex,ufInt32 DisplayColIndex,ufInt32 DisplayColEndIndex)=0;
	
	virtual ufInt32 ViewGridDisplayRowIndexToRowIndex(ufInt32 DisplayRowIndex);
	virtual ufInt32 ViewGridRowIndexToDisplayRowIndex(ufInt32 RowIndex);
	virtual ufInt32 ViewGridDisplayColumnIndexToColumnIndex(ufInt32 DisplayColumnIndex);
	virtual ufInt32 ViewGridColumnIndexToDisplayColumnIndex(ufInt32 ColumnIndex);

	void SetNeedUpdateGridViews(void);
	void SetNeedUpdateGridLayout(void);

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

	void RowDeleted(cRow *Row);

	void PositRow(cRow *Row);
	void PositCell(cCell *Cell);
	
};

typedef bcViewGrid::cRow cViewGridRow;
typedef bcViewGrid::cCell cViewGridCell;
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlListSource
{
public:
	cFunctionSet<void (void)> ControlListNotifySet;

	virtual bool ListGetItemCount(ufInt32 &ItemCount)=0;
	virtual void ListSetupItems(ufInt32 RowIndex,ufInt32 RowCount)=0;
	virtual void ListClearItems(void)=0;
	virtual Float32 ListItemGetSize(ufInt32 RowIndex)=0;
	virtual Float32 ListItemGetDistance(ufInt32 RowIndex)=0;
	virtual rPtr<iViewReference> ListItemMake(cScrollViewContainer *Container,ufInt32 ItemIndex)=0;
};
//---------------------------------------------------------------------------
class cControlList : public bcViewList
{
public:
	cControlList();
	~cControlList();

	iControlListSource* GetControlSource(void)const;
	void SetControlSource(iControlListSource *Source);


	Float32 ItemSize=0;
	Float32 ItemBorder=0;
protected:
	iControlListSource *fControlSource=nullptr;


	// bcViewList

	virtual void ScrollContentUpdateContentLayout(void)override;
	
	virtual cUIRange ViewListDisplayQueryRange(ufInt32 DisplayIndex)override;
	virtual bool ViewListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)override;
	virtual void ViewListDisplaySetupItems(ufInt32 DisplayIndex,ufInt32 DisplayEndIndex)override;
	
	// DisplayIndex - ItemIndex

private:

	iFunctionToken *fListNotifyToken;
	void ApplyControlSource(void);
	ufInt32 fListItemCount;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlGridSource
{
public:
	cFunctionSet<void (void)> ControlGridNotifySet;

	virtual ufInt16 GridColumnCount(void)=0;
	virtual Float32 GridColumnGetSize(ufInt16 Index)=0;
	virtual Float32 GridColumnGetDistance(ufInt16 Index)=0;

	virtual bool GridGetRowCount(ufInt32 &RowCount)=0;
	virtual void GridSetupRows(ufInt32 RowIndex,ufInt32 RowCount)=0;
	virtual void GridClearRows(void)=0;
	virtual Float32 GridRowGetSize(ufInt32 RowIndex)=0;
	virtual Float32 GridRowGetDistance(ufInt32 RowIndex)=0;
	virtual rPtr<iViewReference> GridRowMakeCell(cScrollViewContainer *Container,ufInt32 RowIndex,ufInt32 ColIndex)=0;
};
//---------------------------------------------------------------------------
class cControlGrid : public bcViewGrid
{
public:
	cControlGrid();
	~cControlGrid();

	iControlGridSource* GetControlSource(void)const;
	void SetControlSource(iControlGridSource *Source);


	Float32 RowSize=0;
	Float32 RowBorder=0;
protected:
	iControlGridSource *fControlSource;


	// bcViewGrid

	virtual void ScrollContentUpdateContentLayout(void)override;
	
	virtual cUIRange ViewGridDisplayRowQueryRange(ufInt32 RowIndex)override;
	virtual cUIRange ViewGridDisplayColumnQueryRange(ufInt32 ColumnIndex)override;
	virtual bool ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &RowIndex)override;
	virtual bool ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)override;
	virtual void ViewGridDisplaySetupRows(bool UpdateRow,ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)override;

private:
	iFunctionToken *fGridNotifyToken;

	void ApplyControlSource(void);
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
	cControlGridList();
	~cControlGridList();

	Float32 ColumnSize=40;
	Float32 ColumnBorder=0;
	Float32 RowSize=40;
	Float32 RowBorder=1;


	iControlListSource* GetControlSource(void)const;
	void SetControlSource(iControlListSource *Source);


	ufInt16 GetColumnCount(void)const;

protected:
	iControlListSource *fControlSource;
	
	virtual void cnLib_FUNC RectangleChanged(bool Moved,bool Sized)override;

	// bcViewGrid
	
	virtual void ScrollContentUpdateContentLayout(void)override;
	
	virtual cUIRange ViewGridDisplayRowQueryRange(ufInt32 RowIndex)override;
	virtual cUIRange ViewGridDisplayColumnQueryRange(ufInt32 ColumnIndex)override;
	virtual bool ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &RowIndex)override;
	virtual bool ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)override;
	virtual void ViewGridDisplaySetupRows(bool UpdateRow,ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)override;

private:
	
	iFunctionToken *fListNotifyToken;
	ufInt16 fColumnCount;

	void ApplyControlSource(void);
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
