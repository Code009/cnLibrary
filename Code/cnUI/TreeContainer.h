/*- cnUI - Tree Container -------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2018-10-25                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLib_cnUI_TreeContainer_H__
#define __cnLib_cnUI_TreeContainer_H__
/*-------------------------------------------------------------------------*/
#include <cnUI/ControlBase.h>
#include <cnUI/ScrollContainer.h>
#include <cnUI/GridContainer.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnUI{
//---------------------------------------------------------------------------
class cnLib_INTERFACE viTreeBranchData : public viData
{
public:
	cFunctionSet<void (void)noexcept(true)> TreeBranchNotifySet;
	struct cTreeBranchInfo{
		ufInt16 BranchCount;
		eDirection BranchDirection;
	};
	virtual cTreeBranchInfo TreeBranchInfo(void)noexcept(true)=0;

	struct cTreeBranchItem
	{
		cUIRectangle Rect;
		ufInt16 NodeCount;
		const Float32 *NodeOffsets;
	};
	virtual cTreeBranchItem TreeBranchItem(ufInt16 BranchIndex)noexcept(true)=0;
};
//---------------------------------------------------------------------------
extern rPtr<viControl> (*gCreateDefaultTreeBranchVisual)(viTreeBranchData *Data)noexcept(true);
extern rPtr<viControl> (*gCreateDefaultTreeExpandButtonVisual)(viButtonData *Data)noexcept(true);
//---------------------------------------------------------------------------
class bcViewTreeContainer : public cScrollViewContainer, public viTreeBranchData
{
public:
	bcViewTreeContainer()noexcept(true);
	~bcViewTreeContainer()noexcept(true);

	virtual cTreeBranchInfo TreeBranchInfo(void)noexcept(true)override;
	virtual cTreeBranchItem TreeBranchItem(ufInt16 BranchIndex)noexcept(true)override;

	eDirection GetTreeDirection(void)const noexcept(true);
	void SetTreeDirection(eDirection Direction)noexcept(true);
	Float32 GetTreeLevelIdent(void)const noexcept(true);
	void SetTreeLevelIdent(Float32 LevelIdent)noexcept(true);

	void SetTreeBranchContent(rPtr<viControl> Content)noexcept(true);
	Float32 TreeBranchContentZPosition=ZPosition_Content+1;
	
	virtual void UpdateControlZOffset(void)noexcept(true)override;

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	union{	// 0=vertial tree  1=horizontal tree
		bool fTreeXBranch;
		bool fTreeYNode;
	};
	bool fTreeBranchUpper;

	rPtr<viControl> fTreeBranchContent;
	virtual void ControlTreeBranchContentSetDefault(void)noexcept(true);
	Float32 fTreeLevelIndent;

	//struct cTreeNodeInfo
	//{
	//	cnRTL::aClsRef<cTreeNodeInfo> Parent;
	//	uIntn Index;
	//	sfInt16 Level;
	//	bool IsLastNode;
	//};

	//virtual aClsRef<cTreeNodeInfo> TreeNodeGetInfo(uIntn VisibleIndex)const=0;

	struct BuildContext
	{
		cUIPoint TreePos;
		Float32 LevelIndent;
		bool GridXRow;
	};

	void TreeBranchDataClear(void)noexcept(true);
	void TreeBranchDataBuildStart(cUIRange ContentRange)noexcept(true);
	void TreeBranchDataBuildBranch(BuildContext &BuildContext,cUIRange ItemRange,sfInt16 Level,bool IsLastNode)noexcept(true);
	void TreeBranchDataBuildFinish(BuildContext &BuildContext)noexcept(true);

private:

	// tree branch graph
	
	struct cTreeBranch
	{
		cUIRectangle Rect;
		//cUIRange Range;
		//Float32 SecondaryPos;
		bool LevelFinished;
		cnRTL::cSeqList<Float32> NodeOffsetList;
	};
	cnRTL::cSeqList<cTreeBranch> fTreeBranchs;

	cUIRange fBuildingContentRange;
	Float32 fBuildingLastNodeEndPos;
	cnRTL::cSeqList<cTreeBranch> fBuildingBranchs;

};
//---------------------------------------------------------------------------
class cControlTreeBranchData
{
public:
	cControlTreeBranchData()noexcept(true);
	~cControlTreeBranchData()noexcept(true);

	uIntn GetBranchCount(void)const noexcept(true);
	viTreeBranchData::cTreeBranchItem GetBranchItem(uIntn BranchIndex)const noexcept(true);

	void Clear(void)noexcept(true);


	struct BuildContext
	{
		cUIPoint TreePos;
		Float32 LevelIndent;
		bool GridXRow;
	};

	void BuildStart(cUIRange ContentRange)noexcept(true);
	void BuildBranch(BuildContext &BuildContext,cUIRange ItemRange,sfInt16 Level,bool IsLastNode)noexcept(true);
	void BuildFinish(BuildContext &BuildContext)noexcept(true);
private:
	bool fTreeHorizontal;

	struct cTreeBranch
	{
		cUIRectangle Rect;
		//cUIRange Range;
		//Float32 SecondaryPos;
		bool LevelFinished;
		cnRTL::cSeqList<cUIPoint> NodeList;
	};
	cnRTL::cSeqList<cTreeBranch> fTreeBranchs;

private:

	cUIRange fBuildingContentRange;
	Float32 fBuildingLastNodeEndPos;
	cnRTL::cSeqList<cTreeBranch> fBuildingBranchs;

};
//---------------------------------------------------------------------------
class bcViewTreeList : public bcViewTreeContainer, public cGridLineData
{
public:
	bcViewTreeList()noexcept(true);
	~bcViewTreeList()noexcept(true);


	Float32 GetListOffset(void)const noexcept(true);
	void SetListOffset(Float32 ListOffset)noexcept(true);

	class cNode : public bcViewItem
	{
	public:
		cNode()noexcept(true);
		~cNode()noexcept(true);
		iViewReference* GetViewReference(void)const noexcept(true);
		void SetViewReference(iViewReference *ViewReference)noexcept(true);

		aClsRef<cNode> QueryNode(uIntn NodeIndex)noexcept(true);
		aCls<cNode>* GetParent(void)const noexcept(true);
	private:
		friend bcViewTreeList;
		bcViewTreeList *fOwner;
		cNode *fParent;
		cnRTL::cSeqMap<ufInt32,aCls<cNode>*> fNodeMap;
		ufInt32 fNodeIndex;
		//ufInt32 fTreeIndex;

		void SetupOwner(bcViewTreeList *Owner,cNode *Parent,ufInt32 NodeIndex)noexcept(true);
		void ClearOwner(void)noexcept(true);

		void NodeDeleted(cNode *Node)noexcept(true);

	};

	aCls<cNode>* QueryRootNode(void)noexcept(true);
	aCls<cNode>* FindNode(Float32 Position)noexcept(true);
	bool FindNode(Float32 Position,aCls<cNode>* &Node)noexcept(true);

protected:

	virtual void ViewSetup(void)noexcept(true)override;
	virtual void ViewClear(void)noexcept(true)override;

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	// content range - display index

	virtual cUIRange ViewTreeListDisplayQueryRange(ufInt32 DisplayIndex)noexcept(true)=0;
	virtual bool ViewTreeListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)noexcept(true)=0;
	virtual void ViewTreeListDisplaySetupItems(uIntn DisplayIndex,ufInt32 DisplayEndIndex)noexcept(true)=0;

	// display index - node

	virtual ufInt32 ViewTreeListNodeToDisplayIndex(cNode *Node)noexcept(true)=0;
	virtual aClsRef<cNode> ViewTreeListDisplayIndexToNode(ufInt32 DisplayIndex)noexcept(true)=0;

	void SetNeedUpdateTreeViews(void)noexcept(true);
	void SetNeedUpdateTreeLayout(void)noexcept(true);

private:
	bool fInLayout;
	bool fNeedUpdateTreeViews;
	bool fNeedUpdateTreeLayout;

	ufInt32 fViewportDisplayIndex;
	ufInt32 fViewportDisplayEndIndex;

	aClsRef<cNode> fRootNode;

	cnRTL::cSeqSet< aClsRef<cNode> > fInUseNodeSet;

	void PositNode(cNode *Node)noexcept(true);
	void TreePositNode(cNode *Node)noexcept(true);
	void TreeKeepVisibleSubNode(cNode *Node)noexcept(true);
	
};

//---------------------------------------------------------------------------
class bcTreeViewGrid : public bcViewTreeContainer, public cGridLineData
{
public:
	bcTreeViewGrid();
	~bcTreeViewGrid();

	bool GetVerticalColumn(void)const;
	void SetVerticalColumn(bool Value);
	Float32 GetRowOffset(void)const;
	Float32 GetColumnOffset(void)const;

	void SetGridOffset(Float32 RowOffset,Float32 ColumnOffset);

	Float32 GetRowHeadMargin(void)const;
	Float32 GetRowTailMargin(void)const;
	Float32 GetColumnHeadMargin(void)const;
	Float32 GetColumnTailMargin(void)const;

	class cRow;
	class cCell
	{
	public:
		cCell(cRow *Owner,ufInt32 ColumnIndex);
		~cCell();
		iViewReference* GetViewReference(void)const;
		void SetViewReference(iViewReference *ViewReference);
	private:
		friend bcTreeViewGrid;
		friend cRow;
		cRow *fOwner;
		aClsRef<cScrollViewItem> fViewItem;
		ufInt32 fColumnIndex;
	};
	class cRow
	{
	public:
		cRow(bcTreeViewGrid *Owner,ufInt32 RowIndex);
		~cRow();
		iViewReference* GetViewReference(void)const;
		void SetViewReference(iViewReference *ViewReference);
		aClsRef<cCell> QueryCell(ufInt32 ColumnIndex);
	private:
		friend bcTreeViewGrid;
		bcTreeViewGrid *fOwner;
		aClsRef<cScrollViewItem> fViewItem;
		ufInt32 fRowIndex;
		cnRTL::cSeqMap<ufInt32,aCls<cCell>*> fCellMap;

		void AttachCell(aCls<cCell> *Cell);
		void DetachCell(cCell *Cell);
	};
	aClsRef<cRow> QueryRow(ufInt32 RowIndex);


	cUIRange QueryRowRange(ufInt32 RowIndex);
	cUIRange QueryColumnRange(ufInt32 ColumnIndex);

	ufInt32 FindRowIndexAt(Float32 RowPosition);
	ufInt32 FindColumnIndexAt(Float32 ColumnPosition);

	bool FindRowIndexAt(Float32 RowPosition,ufInt32 &RowIndex);
	bool FindColumnIndexAt(Float32 ColumnPosition,ufInt32 &ColumnIndex);

protected:

	virtual void cnLib_FUNC UILayout(void)noexcept(true)override;

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;

	// control list

	bool fGridMoreRows;


	virtual cUIRange ScrollViewGridRowQueryRange(ufInt32 RowIndex)=0;
	virtual cUIRange ScrollViewGridColumnQueryRange(ufInt32 RowIndex)=0;
	virtual bool ScrollViewGridRowFindAt(Float32 RowPosition,ufInt32 &RowIndex)=0;
	virtual bool ScrollViewGridColumnFindAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)=0;
	virtual void ScrollViewGridSetupRows(ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)=0;
	virtual void ScrollViewGridSetupCells(ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)=0;
	
	void SetNeedUpdateGridLayout(void)noexcept(true);

private:
	bool fNeedUpdateGridLayout;

	struct cColumnItem
	{
		Float32 Begin;
		Float32 End;
	};
	cnRTL::cSeqList<cColumnItem> fColumns;
	ufInt32 fColumnVisibleIndex;
	ufInt32 fColumnVisibleEndIndex;

	ufInt32 fRowVisibleIndex;
	ufInt32 fRowVisibleEndIndex;

	cnRTL::cSeqMap<ufInt32,aCls<cRow>*> fRowMap;

	cnRTL::cSeqSet< aClsRef<cRow> > fInUseRowSet;
	cnRTL::cSeqSet< aClsRef<cCell> > fInUseCellSet;

	void AttachRow(aCls<cRow> *Row)noexcept(true);
	void DetachRow(cRow *Row)noexcept(true);

	void PositRow(cRow *Row)noexcept(true);
	void PositCell(cCell *Cell)noexcept(true);
	
};

typedef bcTreeViewGrid::cRow cTreeViewGridRow;
typedef bcTreeViewGrid::cCell cTreeViewGridCell;
//---------------------------------------------------------------------------
class cTreeExpandButton : public cButton
{
public:
protected:
	virtual void ButtonBackgroundSetDefault(void)noexcept(true)override;
};
//---------------------------------------------------------------------------
class cnLib_INTERFACE iControlTreeListSource
{
public:
	cFunctionSet<void (void)noexcept(true)> ControlListNotifySet;

	virtual bool TreeListGetItemCount(ufInt32 &ItemCount)noexcept(true)=0;
	virtual void TreeListSetupItems(ufInt32 RowIndex,ufInt32 RowCount)noexcept(true)=0;
	virtual void TreeListClearItems(void)noexcept(true)=0;
	virtual Float32 TreeListItemGetSize(ufInt32 RowIndex)noexcept(true)=0;
	virtual Float32 TreeListItemGetDistance(ufInt32 RowIndex)noexcept(true)=0;
	virtual rPtr<iViewReference> TreeListItemMake(cScrollViewContainer *Container,ufInt32 ItemIndex)noexcept(true)=0;

	virtual void TreeEnumReset(void)noexcept(true)=0;
	virtual bool TreeEnumRoot(uIntn StartIndex)noexcept(true)=0;
	virtual bool TreeEnumEstimateNodeCount(uIntn &ItemCount)noexcept(true)=0;
	virtual bool TreeEnumNext(void)noexcept(true)=0;
	virtual Float32 TreeEnumGetSize(void)noexcept(true)=0;
	virtual Float32 TreeEnumGetDistance(void)noexcept(true)=0;
	//virtual rPtr<iControlReference> TreeEnumCreate(iControlComponentCache *Cache)noexcept(true)=0;
	virtual bool TreeEnumEnterNode(uIntn StartIndex)noexcept(true)=0;
	virtual bool TreeEnumLeaveNode(void)noexcept(true)=0;
	virtual bool TreeEnumIsExpanded(void)noexcept(true)=0;
	//virtual rPtr<iControlReference> TreeEnumCreateExpandControl(iControlComponentCache *Cache)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cControlTreeList : public bcViewTreeList
{
public:
	cControlTreeList()noexcept(true);
	~cControlTreeList()noexcept(true);

	iControlTreeListSource* GetControlSource(void)const noexcept(true);
	void SetControlSource(iControlTreeListSource *Source)noexcept(true);


	Float32 ItemSize=0;
	Float32 ItemBorder=0;
protected:
	iControlTreeListSource *fControlSource;

	// bcViewTree

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;
	
	virtual cUIRange ViewTreeListDisplayQueryRange(ufInt32 DisplayIndex)noexcept(true)override;
	virtual bool ViewTreeListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)noexcept(true)override;
	virtual void ViewTreeListDisplaySetupItems(uIntn DisplayIndex,ufInt32 DisplayEndIndex)noexcept(true)override;
	virtual ufInt32 ViewTreeListNodeToDisplayIndex(cNode *Node)noexcept(true)override;
	virtual aClsRef<cNode> ViewTreeListDisplayIndexToNode(ufInt32 DisplayIndex)noexcept(true)override;


private:
	iFunctionToken *fTreeListNotifyToken;
	void ApplyControlSource(void)noexcept(true);
	ufInt32 fTreeItemCount;
};
//---------------------------------------------------------------------------
class cControlTreeGrid : public bcTreeViewGrid
{
public:
	cControlTreeGrid()noexcept(true);
	~cControlTreeGrid()noexcept(true);

	//iControlTreeSource* GetControlSource(void)const;
	//void SetControlSource(iControlTreeSource *Source);


	static constexpr Float32 ItemSize=80*4;
protected:
	//iControlTreeSource *fControlSource=nullptr;

	iFunctionToken *fTreeNotifyToken;

	// bcViewTree

	virtual void ScrollContentUpdateContentLayout(void)noexcept(true)override;
	


private:
};
#if 0
//---------------------------------------------------------------------------
class cnLib_DEPRECATED iControlTreeSource1
{
public:
	cCallbackSet TreeNotifySet;

	virtual void TreeEnumReset(void)=0;
	virtual bool TreeEnumRoot(uIntn StartIndex)=0;
	virtual bool TreeEnumEstimateNodeCount(uIntn &ItemCount)=0;
	virtual bool TreeEnumNext(void)=0;
	virtual Float32 TreeEnumGetSize(void)=0;
	virtual Float32 TreeEnumGetDistance(void)=0;
	//virtual rPtr<iControlReference> TreeEnumCreate(iControlComponentCache *Cache)=0;
	virtual bool TreeEnumEnterNode(uIntn StartIndex)=0;
	virtual bool TreeEnumLeaveNode(void)=0;
	virtual bool TreeEnumIsExpanded(void)=0;
	//virtual rPtr<iControlReference> TreeEnumCreateExpandControl(iControlComponentCache *Cache)=0;
};
//---------------------------------------------------------------------------
class cnLib_DEPRECATED iControlTreeGridSource : public iControlTreeSource1
{
public:
	virtual uIntn TreeColumnCount(void)=0;
	virtual Float32 TreeColumnGetSize(uIntn Index)=0;
	virtual Float32 TreeColumnGetDistance(uIntn Index)=0;
	//virtual rPtr<iControlReference> TreeEnumCreate(iControlComponentCache *Cache)override;
	//virtual rPtr<iControlReference> TreeEnumCreateCell(uIntn Index,iControlComponentCache *Cache)=0;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct cIndexRange
{
	uIntn Begin;
	uIntn End;
};
class cArrayAccumulationCache
{
public:
	cArrayAccumulationCache();
	~cArrayAccumulationCache();

	void Clear(void);

	typedef Float32 TValue;
	void UpdateArrayValue(uIntn Index,uIntn Count,const TValue *Values);
	void UpdateSummation(uIntn Index,TValue Summation);
	//TValue FindLowwerSummationAt(uIntn &Index);
	cIndexRange GetSummationRangeAt(uIntn Index,TValue &RangeLowwer,TValue &RangeUpper);
	cIndexRange FindSummationIndexRange(TValue &RangeLowwer,TValue &RangeUpper,TValue Summation);
	//uIntn FindLowwerIndexWithSummation(TValue &FoundSummation,TValue Summation){	return 0;	}
	//uIntn FindUpperIndexWithSummation(TValue &FoundSummation,TValue Summation){	return 0;	}

	void debugUpdateValue(void);

protected:
	
	struct cRangeItem : public cnRTL::cRTLAllocator
	{
		cRangeItem *Parent;
		cnRTL::autoPtr<cRangeItem> Lowwer;	// lowwer branch
		cnRTL::autoPtr<cRangeItem> Upper;	// upper branch
		TValue LowwerSummation;				// summation of lowwer branch
		bool LowwerSummationCompleted;		// wether summation of lowwer branch includes all values
	};
	cRangeItem fRoot;
	ufInt8 fRootBits;
	uIntn ScaleRoot(uIntn LeastIndex);

private:
	cRangeItem* GetItem(uIntn Index,ufInt8 &Level);
	void SetValue(uIntn Index,TValue Value);
};

#endif // 0
//---------------------------------------------------------------------------
}	// namespace cnUI
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif
