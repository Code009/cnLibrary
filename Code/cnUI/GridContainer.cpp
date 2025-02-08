#include "GridContainer.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cGridLineData::cGridLineInfo cGridLineData::GridLineInfo(void)noexcept(true)
{
	cGridLineInfo Info;
	Info.HorizontalItemCount=fLines[0].GetCount();
	Info.VerticalItemCount=fLines[1].GetCount();
	return Info;
}
//---------------------------------------------------------------------------
cUIRange cGridLineData::GridLineRange(bool Vertical,ufInt16 Index)noexcept(true)
{
	auto &Lines=fLines[Vertical];
	auto &Line=Lines[Index];
	return Line.Range;
}
//---------------------------------------------------------------------------
ufInt16 cGridLineData::GridLinePartCount(bool Vertical,ufInt16 Index)noexcept(true)
{
	auto &Lines=fLines[Vertical];
	auto &Line=Lines[Index];
	return Line.Parts.GetCount();
}
//---------------------------------------------------------------------------
cUIRange cGridLineData::GridLinePartRange(bool Vertical,ufInt16 Index,ufInt16 PartIndex)noexcept(true)
{
	auto &Lines=fLines[Vertical];
	auto &Line=Lines[Index];
	return Line.Parts[PartIndex];
}
//---------------------------------------------------------------------------
void cGridLineData::GridLineSetupStart(ufInt16 HorizontalCount,ufInt16 VerticalCount,cUIRange HoriRange,cUIRange VertRange)noexcept(true)
{
	auto &HLines=fLines[0];
	auto &VLines=fLines[1];
	HorizontalCount++;
	VerticalCount++;
	HLines.SetCount(HorizontalCount);
	VLines.SetCount(VerticalCount);

	HLines[0].Range.Begin=HoriRange.Begin;
	HLines[HorizontalCount-1].Range.End=HoriRange.End;
	VLines[0].Range.Begin=VertRange.Begin;
	VLines[VerticalCount-1].Range.End=VertRange.End;
	for(uIntn i=0;i<HorizontalCount;i++){
		auto &Line=HLines[i];
		Line.Gaps.SetCount(VerticalCount);
		cnMemory::ZeroFill(Line.Gaps.GetItems(),VerticalCount*sizeof(bool));
	}
	for(uIntn i=0;i<VerticalCount;i++){
		auto &Line=VLines[i];
		Line.Gaps.SetCount(HorizontalCount);
		cnMemory::ZeroFill(Line.Gaps.GetItems(),HorizontalCount*sizeof(bool));
	
	}
}
//---------------------------------------------------------------------------
void cGridLineData::GridLineSetupFinish(void)noexcept(true)
{
	for(ufInt8 di=0;di<2;di++){
		auto &Lines=fLines[di];
		auto &ALines=fLines[!di];

		auto ItemCount=Lines.GetCount();
		auto AItemCount=ALines.GetCount();

		for(uIntn i=0;i<ItemCount;i++){
			auto &Line=Lines[i];

			Line.Parts.Clear();
		
			if(Line.Range.Begin<Line.Range.End){
				continue;
			}
			ufInt16 PartStart=0;

			for(ufInt16 PartIndex=0;PartIndex<AItemCount;PartIndex++){
				if(Line.Gaps[PartIndex]){
					if(PartStart!=PartIndex){
						cUIRange PartRange;
						PartRange.Begin=ALines[PartStart].Range.Begin;
						PartRange.End=ALines[PartIndex].Range.End;
						Line.Parts.AppendMake(PartRange);
					}
					PartStart=PartIndex+1;
				}
			}
			if(PartStart!=AItemCount){
				cUIRange PartRange;
				PartRange.Begin=ALines[PartStart].Range.Begin;
				PartRange.End=ALines[AItemCount-1].Range.End;
				Line.Parts.AppendMake(PartRange);
			}
		}
	}
	for(ufInt8 di=0;di<2;di++){
		auto &Lines=fLines[di];

		auto ItemCount=Lines.GetCount();

		uIntn MoveIndex=0;
		for(uIntn i=0;i<ItemCount;i++){
			auto &Line=Lines[i];

			if(Line.Range.Begin<Line.Range.End){
				if(MoveIndex!=i){
					Lines[MoveIndex]=cnVar::MoveCast(Lines[i]);
				}
				MoveIndex++;
			}
		}
		if(MoveIndex!=ItemCount){
			Lines.SetCount(MoveIndex);
		}
	}
}
//---------------------------------------------------------------------------
void cGridLineData::GridLineSetupRange(bool Vertical,ufInt16 Index,const cUIRange &Range)noexcept(true)
{
	auto &Lines=fLines[Vertical];
	Lines[Index].Range.End=Range.Begin;
	Lines[Index+1].Range.Begin=Range.End;
}
//---------------------------------------------------------------------------
void cGridLineData::GridLineSetupGap(bool Vertical,ufInt16 Index,ufInt16 GapIndex,ufInt16 GapEndIndex)noexcept(true)
{
	auto &Lines=fLines[Vertical];
	auto &Line=Lines[Index];

	for(ufInt16 gi=GapIndex;gi<GapEndIndex;gi++){
		Line.Gaps[gi]=true;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcViewList::bcViewList()noexcept(true)
	: fListXItem(false)
	, fInLayout(false)
	, fNeedUpdateListViews(false)
	, fNeedUpdateListLayout(false)
	, fViewportDisplayIndex(0)
	, fViewportDisplayEndIndex(0)
{
}
//---------------------------------------------------------------------------
bcViewList::~bcViewList()noexcept(true)
{
	SetView(nullptr);
	InvalidateData();

	for(auto ItemPair : fItemMap){
		ItemPair.Value->ClearOwner();
	}
}
//---------------------------------------------------------------------------
bool bcViewList::GetHorizontalList(void)const noexcept(true)
{
	return fListXItem;
}
//---------------------------------------------------------------------------
void bcViewList::SetHorizontalList(bool Value)noexcept(true)
{
	if(fListXItem==Value)
		return;

	fListXItem=Value;
	SetNeedUpdateListLayout();
}
//---------------------------------------------------------------------------
Float32 bcViewList::GetListOffset(void)const noexcept(true)
{
	return fScrollOffset[!fListXItem];
}
//---------------------------------------------------------------------------
void bcViewList::SetListOffset(Float32 ListOffset)noexcept(true)
{
	Float32 Offset[2];
	Offset[!fListXItem]=ListOffset;
	SetScrollOffset({Offset[0],Offset[1]});
}
//---------------------------------------------------------------------------
ufInt32 bcViewList::FindItemIndexAt(Float32 Position)noexcept(true)
{
	ufInt32 ItemIndex;
	FindItemIndexAt(Position,ItemIndex);
	return ItemIndex;
}
//---------------------------------------------------------------------------
bool bcViewList::FindItemIndexAt(Float32 Position,ufInt32 &ItemIndex)noexcept(true)
{
	ufInt32 DisplayIndex;
	bool Ret=ViewListDisplayFindAt(Position,DisplayIndex);
	ItemIndex=ViewListDisplayIndexToItemIndex(DisplayIndex);
	return Ret;
}
//---------------------------------------------------------------------------
bool bcViewList::IsItemVisible(ufInt32 ItemIndex)const noexcept(true)
{
	auto ItemPair=fItemMap.GetPair(ItemIndex);
	if(ItemPair==nullptr){
		return false;
	}

	return ItemPair->Value->IsVisible();
}
//---------------------------------------------------------------------------
void bcViewList::SetNeedUpdateListLayout(void)noexcept(true)
{
	if(fInLayout || fNeedUpdateListLayout)
		return;

	fNeedUpdateListLayout=true;
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
void bcViewList::SetNeedUpdateListViews(void)noexcept(true)
{
	if(fInLayout || fNeedUpdateListViews)
		return;

	fNeedUpdateListViews=true;
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
ufInt32 bcViewList::ViewListItemIndexToDisplayIndex(ufInt32 ItemIndex)noexcept(true)
{
	return ItemIndex;
}
//---------------------------------------------------------------------------
ufInt32 bcViewList::ViewListDisplayIndexToItemIndex(ufInt32 DisplayIndex)noexcept(true)
{
	return DisplayIndex;
}
//---------------------------------------------------------------------------
void bcViewList::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	fInLayout=true;

	cScrollViewContainer::ScrollContentUpdateContentLayout();
	cnLib_ASSERT(fView!=nullptr);

	// Item visible range
	cUIRange ViewportRange;
	ViewportRange.Begin=fScrollOffset[!fListXItem];
	ViewportRange.End=fScrollOffset[!fListXItem]+fScrollViewportSize[!fListXItem];

	ufInt32 RecycleDisplayIndex=fViewportDisplayIndex;
	ufInt32 RecycleDisplayEndIndex=fViewportDisplayEndIndex;
	
	ViewListDisplayFindAt(ViewportRange.Begin,fViewportDisplayIndex);
	if(ViewListDisplayFindAt(ViewportRange.End,fViewportDisplayEndIndex)){
		fViewportDisplayEndIndex++;
	}


	{
		ufInt16 ItemCount=fViewportDisplayEndIndex-fViewportDisplayIndex;
		cUIRange ListRange;
		ListRange.Begin=fScrollLowwerLimit[fListXItem]-fScrollOffset[fListXItem];
		ListRange.End=fScrollUpperLimit[fListXItem]-fScrollOffset[fListXItem];

		cUIRange VisibleRange;
		VisibleRange.Begin=ViewportRange.Begin-fScrollOffset[!fListXItem];
		VisibleRange.End=ViewportRange.End-fScrollOffset[!fListXItem];
		if(fListXItem){
			GridLineSetupStart(ItemCount,1,VisibleRange,ListRange);
		}
		else{
			GridLineSetupStart(1,ItemCount,ListRange,VisibleRange);
		}

		for(ufInt16 i=0;i<ItemCount;i++){
			auto ItemRange=ViewListDisplayQueryRange(i+fViewportDisplayIndex);
			ItemRange.Begin-=fScrollOffset[!fListXItem];
			ItemRange.End-=fScrollOffset[!fListXItem];
			GridLineSetupRange(!fListXItem,i,ItemRange);
		}
		GridLineSetupRange(fListXItem,0,ListRange);
		GridLineSetupFinish();
	}

	if(fNeedUpdateListLayout){
		fNeedUpdateListLayout=false;

		for(auto ItemPair : fItemMap){
			PositItem(ItemPair.Value);
		}
	}

	// check visibility
	for(auto Pair : fItemMap){
		if(Pair.Key<fViewportDisplayIndex)
			continue;
		if(Pair.Key>=fViewportDisplayEndIndex)
			break;
		ViewItemCheckVisible(Pair.Value);
	}

	if(fNeedUpdateListViews){
		fNeedUpdateListViews=false;
		// update all
		ViewListDisplaySetupItems(fViewportDisplayIndex,fViewportDisplayEndIndex);
	}
	else{

		if(fViewportDisplayIndex<RecycleDisplayIndex){
			if(fViewportDisplayEndIndex>RecycleDisplayEndIndex){
				//     |    |		PrevVisible
				// |---|////|---|	NewVisible
				//  Sec1	Sec2

				// Sec1
				ViewListDisplaySetupItems(fViewportDisplayIndex,RecycleDisplayIndex);

				// Sec2
				ViewListDisplaySetupItems(RecycleDisplayEndIndex,fViewportDisplayEndIndex);

				// no change to recycle section
			}
			else if(fViewportDisplayEndIndex>RecycleDisplayIndex){
				//     |    |	PrevVisible
				// |---|///|	NewVisible

				ViewListDisplaySetupItems(fViewportDisplayIndex,RecycleDisplayIndex);

				RecycleDisplayEndIndex=fViewportDisplayEndIndex;
			}
			else{
				//		|   |	PrevVisible
				// |---|		NewVisible

				ViewListDisplaySetupItems(fViewportDisplayIndex,fViewportDisplayEndIndex);
				
				RecycleDisplayIndex=0;
				RecycleDisplayEndIndex=0;
			}
		}
		else if(fViewportDisplayIndex<RecycleDisplayEndIndex){
			if(fViewportDisplayEndIndex>RecycleDisplayEndIndex){
				// |    |		PrevVisible
				//  |///|---|	NewVisible
				
				ViewListDisplaySetupItems(RecycleDisplayEndIndex,fViewportDisplayEndIndex);

				RecycleDisplayIndex=fViewportDisplayIndex;
			}
			else{
				// |    |		PrevVisible
				//  |//|		NewVisible
				
				RecycleDisplayIndex=fViewportDisplayIndex;
				RecycleDisplayEndIndex=fViewportDisplayEndIndex;
			}
		}
		else{
			// |   |		PrevVisible
			//       |---|	NewVisible
				
			ViewListDisplaySetupItems(fViewportDisplayIndex,fViewportDisplayEndIndex);
				
			RecycleDisplayIndex=0;
			RecycleDisplayEndIndex=0;
		}

	}

	auto PrevInUseItemSet=cnVar::MoveCast(fInUseItemSet);

	for(auto ItemPair : fItemMap){
		if(ItemPair.Value->IsVisible()){
			fInUseItemSet.Insert(ItemPair.Value);
		}
		else{
			ufInt32 ItemIndex=ItemPair.Key;
			if(ItemIndex>=fViewportDisplayIndex && ItemIndex<fViewportDisplayEndIndex){
				fInUseItemSet.Insert(ItemPair.Value);
			}
		}
	}
	fInLayout=false;
}
//---------------------------------------------------------------------------
aClsRef<bcViewList::cItem> bcViewList::QueryItem(ufInt32 ItemIndex)noexcept(true)
{
	aClsRef<bcViewList::cItem> RetItem;

	auto &CacheItem=fItemMap[ItemIndex];
	if(CacheItem!=nullptr){
		RetItem=CacheItem;
	}
	else{
		RetItem=aClsCreate<cItem>();
		RetItem->SetupOwner(this,ItemIndex);
		PositItem(RetItem);
		CacheItem=RetItem;
	}
	

	// store to visible row to keep it until next refresh
	fInUseItemSet.Insert(RetItem);

	return RetItem;
}
//---------------------------------------------------------------------------
void bcViewList::ItemDeleted(cItem *Item)noexcept(true)
{
	auto Pair=fItemMap.GetPair(Item->fItemIndex);
	if(Pair!=nullptr){
		if(Pair->Value==Item)
			fItemMap.RemovePair(Pair);
	}
}
//---------------------------------------------------------------------------
void bcViewList::PositItem(cItem *Item)noexcept(true)
{
	cUIRectangle ItemRect;

	ufInt32 DisplayIndex=ViewListItemIndexToDisplayIndex(Item->fItemIndex);
	if(DisplayIndex==IndexNotFound)
		return;
	auto ItemRange=ViewListDisplayQueryRange(DisplayIndex);

	if(fListXItem){
		ItemRect.Left=ItemRange.Begin;
		ItemRect.Right=ItemRange.End;

		ItemRect.Top=fScrollLowwerLimit[!fListXItem];
		ItemRect.Bottom=fScrollUpperLimit[!fListXItem];
	}
	else{
		ItemRect.Top=ItemRange.Begin;
		ItemRect.Bottom=ItemRange.End;

		ItemRect.Left=fScrollLowwerLimit[!fListXItem];
		ItemRect.Right=fScrollUpperLimit[!fListXItem];
	}
	
	ViewItemSetRectangle(Item,ItemRect);
}
//---------------------------------------------------------------------------
bcViewList::cItem::cItem()noexcept(true)
	: fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
bcViewList::cItem::~cItem()noexcept(true)
{
	if(fOwner!=nullptr){
		fOwner->ItemDeleted(this);
	}
}
//---------------------------------------------------------------------------
void bcViewList::cItem::SetupOwner(bcViewList *Owner,ufInt32 ItemIndex)noexcept(true)
{
	fOwner=Owner;
	fItemIndex=ItemIndex;
	fOwner->ViewItemAttach(this);
}
//---------------------------------------------------------------------------
void bcViewList::cItem::ClearOwner(void)noexcept(true)
{
	fOwner->ViewItemDetach(this);
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
iViewReference* bcViewList::cItem::GetViewReference(void)const noexcept(true)
{
	if(fOwner==nullptr)
		return nullptr;
	return fOwner->ViewItemGetViewReference(this);
}
//---------------------------------------------------------------------------
void bcViewList::cItem::SetViewReference(iViewReference *ViewRef)noexcept(true)
{
	if(fOwner==nullptr)
		return;
	fOwner->ViewItemSetViewReference(this,ViewRef);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcViewGrid::bcViewGrid()noexcept(true)
	: fGridXRow(false)
	, fInLayout(false)
	, fNeedUpdateGridViews(false)
	, fNeedUpdateGridLayout(false)
	, fViewportDisplayRowIndex(0)
	, fViewportDisplayRowEndIndex(0)
	, fViewportDisplayColumnIndex(0)
	, fViewportDisplayColumnEndIndex(0)
{
}
//---------------------------------------------------------------------------
bcViewGrid::~bcViewGrid()noexcept(true)
{
	SetView(nullptr);
	InvalidateData();

	for(auto RowPair : fRowMap){
		RowPair.Value->ClearOwner();
	}
}
//---------------------------------------------------------------------------
bool bcViewGrid::GetVerticalColumn(void)const noexcept(true)
{
	return fGridXRow;
}
//---------------------------------------------------------------------------
void bcViewGrid::SetVerticalColumn(bool Value)noexcept(true)
{
	if(fGridXRow==Value)
		return;

	fGridXRow=Value;
	SetNeedUpdateGridLayout();
}
//---------------------------------------------------------------------------
Float32 bcViewGrid::GetRowOffset(void)const noexcept(true)
{
	return fScrollOffset[!fGridXRow];
}
//---------------------------------------------------------------------------
Float32 bcViewGrid::GetColumnOffset(void)const noexcept(true)
{
	return fScrollOffset[fGridYColumn];
}
//---------------------------------------------------------------------------
void bcViewGrid::SetGridOffset(Float32 RowOffset,Float32 ColumnOffset)noexcept(true)
{
	Float32 Offset[2];
	Offset[!fGridXRow]=RowOffset;
	Offset[fGridYColumn]=ColumnOffset;
	SetScrollOffset({Offset[0],Offset[1]});
}
//---------------------------------------------------------------------------
ufInt32 bcViewGrid::FindRowIndexAt(Float32 RowPosition)noexcept(true)
{
	ufInt32 RowIndex;
	FindColumnIndexAt(RowPosition,RowIndex);
	return RowIndex;
}
//---------------------------------------------------------------------------
ufInt32 bcViewGrid::FindColumnIndexAt(Float32 ColumnPosition)noexcept(true)
{
	ufInt32 ColIndex;
	FindColumnIndexAt(ColumnPosition,ColIndex);
	return ColIndex;
}
//---------------------------------------------------------------------------
bool bcViewGrid::FindRowIndexAt(Float32 RowPosition,ufInt32 &RowIndex)noexcept(true)
{
	ufInt32 DisplayRowIndex;
	bool Ret=ViewGridDisplayRowFindAt(RowPosition,DisplayRowIndex);
	RowIndex=ViewGridDisplayRowIndexToRowIndex(DisplayRowIndex);
	return Ret;
}
//---------------------------------------------------------------------------
bool bcViewGrid::FindColumnIndexAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)noexcept(true)
{
	ufInt32 DisplayColIndex;
	bool Ret=ViewGridDisplayColumnFindAt(ColumnPosition,DisplayColIndex);
	ColumnIndex=ViewGridDisplayColumnIndexToColumnIndex(DisplayColIndex);
	return Ret;
}
//---------------------------------------------------------------------------
bool bcViewGrid::IsRowVisible(ufInt32 RowIndex)const noexcept(true)
{
	auto RowPair=fRowMap.GetPair(RowIndex);
	if(RowPair==nullptr){
		return false;
	}

	return RowPair->Value->IsVisible();
}
//---------------------------------------------------------------------------
void bcViewGrid::SetNeedUpdateGridViews(void)noexcept(true)
{
	if(fInLayout || fNeedUpdateGridViews)
		return;

	fNeedUpdateGridViews=true;
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
void bcViewGrid::SetNeedUpdateGridLayout(void)noexcept(true)
{
	if(fInLayout || fNeedUpdateGridLayout)
		return;

	fNeedUpdateGridLayout=true;
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
ufInt32 bcViewGrid::ViewGridDisplayRowIndexToRowIndex(ufInt32 DisplayRowIndex)noexcept(true)
{
	return DisplayRowIndex;
}
//---------------------------------------------------------------------------
ufInt32 bcViewGrid::ViewGridRowIndexToDisplayRowIndex(ufInt32 RowIndex)noexcept(true)
{
	return RowIndex;
}
//---------------------------------------------------------------------------
ufInt32 bcViewGrid::ViewGridDisplayColumnIndexToColumnIndex(ufInt32 DisplayColumnIndex)noexcept(true)
{
	return DisplayColumnIndex;
}
//---------------------------------------------------------------------------
ufInt32 bcViewGrid::ViewGridColumnIndexToDisplayColumnIndex(ufInt32 ColumnIndex)noexcept(true)
{
	return ColumnIndex;
}
//---------------------------------------------------------------------------
void bcViewGrid::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	fInLayout=true;
	cScrollViewContainer::ScrollContentUpdateContentLayout();
	cnLib_ASSERT(fView!=nullptr);

	// row visible range
	cUIRange RowVisibleRange;
	RowVisibleRange.Begin=fScrollOffset[!fGridXRow];
	RowVisibleRange.End=fScrollOffset[!fGridXRow]+fScrollViewportSize[!fGridXRow];

	// column visible range
	cUIRange ColVisibleRange;
	ColVisibleRange.Begin=fScrollOffset[fGridYColumn];
	ColVisibleRange.End=fScrollOffset[fGridYColumn]+fScrollViewportSize[fGridYColumn];

	ufInt32 RecycleDisplayRowIndex=fViewportDisplayRowIndex;
	ufInt32 RecycleDisplayRowEndIndex=fViewportDisplayRowEndIndex;

	ufInt32 RecycleDisplayColIndex=fViewportDisplayColumnIndex;
	ufInt32 RecycleDisplayColEndIndex=fViewportDisplayColumnEndIndex;

	ViewGridDisplayRowFindAt(RowVisibleRange.Begin,fViewportDisplayRowIndex);
	if(ViewGridDisplayRowFindAt(RowVisibleRange.End,fViewportDisplayRowEndIndex)){
		fViewportDisplayRowEndIndex++;
	}
	ViewGridDisplayColumnFindAt(ColVisibleRange.Begin,fViewportDisplayColumnIndex);
	if(ViewGridDisplayColumnFindAt(ColVisibleRange.End,fViewportDisplayColumnEndIndex)){
		fViewportDisplayColumnEndIndex++;
	}

	{
		ufInt16 RowCount=fViewportDisplayRowEndIndex-fViewportDisplayRowIndex;
		ufInt16 ColCount=fViewportDisplayColumnEndIndex-fViewportDisplayColumnIndex;
		cUIRange RowRange=RowVisibleRange;
		cUIRange ColRange=ColVisibleRange;

		RowRange.Begin-=fScrollOffset[!fGridXRow];
		RowRange.End-=fScrollOffset[!fGridXRow];

		ColRange.Begin-=fScrollOffset[fGridYColumn];
		ColRange.End-=fScrollOffset[fGridYColumn];

		if(fGridXRow){
			GridLineSetupStart(RowCount,ColCount,RowRange,ColRange);
		}
		else{
			GridLineSetupStart(ColCount,RowCount,ColRange,RowRange);
		}

		for(ufInt16 r=0;r<RowCount;r++){
			RowRange=ViewGridDisplayRowQueryRange(r+fViewportDisplayRowIndex);
			RowRange.Begin-=fScrollOffset[!fGridXRow];
			RowRange.End-=fScrollOffset[!fGridXRow];
			GridLineSetupRange(!fGridXRow,r,RowRange);
		}
		for(ufInt16 c=0;c<ColCount;c++){
			ColRange=ViewGridDisplayColumnQueryRange(c+fViewportDisplayColumnIndex);
			ColRange.Begin-=fScrollOffset[fGridYColumn];
			ColRange.End-=fScrollOffset[fGridYColumn];
			GridLineSetupRange(fGridYColumn,c,ColRange);
		}
		GridLineSetupFinish();
	}

	if(fNeedUpdateGridLayout){
		fNeedUpdateGridLayout=false;

		for(auto RowPair : fRowMap){
			PositRow(RowPair.Value);

			for(auto CellPair : RowPair.Value->fCellMap){
				PositCell(CellPair.Value);
			}
		}
	}

	// check visibility 
	for(auto RowPair : fRowMap){
		if(RowPair.Key<fViewportDisplayRowIndex)
			continue;
		if(RowPair.Key>=fViewportDisplayRowEndIndex)
			break;
		ViewItemCheckVisible(RowPair.Value);

		for(auto CellPair : RowPair.Value->fCellMap){
			if(CellPair.Key<fViewportDisplayColumnIndex)
				continue;
			if(CellPair.Key>=fViewportDisplayColumnEndIndex)
				break;
			ViewItemCheckVisible(CellPair.Value);
		}
	}

	if(fNeedUpdateGridViews){
		fNeedUpdateGridViews=false;
		// update all
		ViewGridDisplaySetupRows(true,fViewportDisplayRowIndex,fViewportDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);
	}
	else{
		if(fViewportDisplayRowIndex<RecycleDisplayRowIndex){
			if(fViewportDisplayRowEndIndex>RecycleDisplayRowEndIndex){
				//     |    |		PrevVisible
				// |---|////|---|	NewVisible
				//  Sec1	Sec2

				// Sec1
				ViewGridDisplaySetupRows(true,fViewportDisplayRowIndex,RecycleDisplayRowIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);

				// Sec2
				ViewGridDisplaySetupRows(true,RecycleDisplayRowEndIndex,fViewportDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);

				// no change to recycle section
			}
			else if(fViewportDisplayRowEndIndex>RecycleDisplayRowIndex){
				//     |    |	PrevVisible
				// |---|///|	NewVisible

				ViewGridDisplaySetupRows(true,fViewportDisplayRowIndex,RecycleDisplayRowIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);

				RecycleDisplayRowEndIndex=fViewportDisplayRowEndIndex;
			}
			else{
				//		|   |	PrevVisible
				// |---|		NewVisible

				ViewGridDisplaySetupRows(true,fViewportDisplayRowIndex,fViewportDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);
				
				RecycleDisplayRowIndex=0;
				RecycleDisplayRowEndIndex=0;
			}
		}
		else if(fViewportDisplayRowIndex<RecycleDisplayRowEndIndex){
			if(fViewportDisplayRowEndIndex>RecycleDisplayRowEndIndex){
				// |    |		PrevVisible
				//  |///|---|	NewVisible
				//       Sec1
				
				// Sec1
				ViewGridDisplaySetupRows(true,RecycleDisplayRowEndIndex,fViewportDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);

				RecycleDisplayRowIndex=fViewportDisplayRowIndex;
			}
			else{
				// |    |		PrevVisible
				//  |//|		NewVisible
				
				RecycleDisplayRowIndex=fViewportDisplayRowIndex;
				RecycleDisplayRowEndIndex=fViewportDisplayRowEndIndex;
			}
		}
		else{
			// |   |		PrevVisible
			//       |---|	NewVisible
				
			ViewGridDisplaySetupRows(true,fViewportDisplayRowIndex,fViewportDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);
				
			RecycleDisplayRowIndex=0;
			RecycleDisplayRowEndIndex=0;
		}

		if(RecycleDisplayRowIndex<RecycleDisplayRowEndIndex){
			// has recycle row
			if(fViewportDisplayColumnIndex<RecycleDisplayColIndex){
				if(fViewportDisplayColumnEndIndex>RecycleDisplayColEndIndex){
					//     |    |		PrevVisible
					// |---|////|---|	NewVisible
					//  Sec1	Sec2
					
					// Sec1
					ViewGridDisplaySetupRows(false,RecycleDisplayRowIndex,RecycleDisplayRowEndIndex,fViewportDisplayColumnIndex,RecycleDisplayColIndex);

					// Sec2
					ViewGridDisplaySetupRows(false,RecycleDisplayRowIndex,RecycleDisplayRowEndIndex,RecycleDisplayColEndIndex,fViewportDisplayColumnEndIndex);
				}
				else if(fViewportDisplayColumnEndIndex>RecycleDisplayColIndex){
					//     |    |	PrevVisible
					// |---|///|	NewVisible

					ViewGridDisplaySetupRows(false,RecycleDisplayRowIndex,RecycleDisplayRowEndIndex,fViewportDisplayColumnIndex,RecycleDisplayColIndex);
				}
				else{
					//		|   |	PrevVisible
					// |---|		NewVisible

					ViewGridDisplaySetupRows(false,RecycleDisplayRowIndex,RecycleDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);
				}
			}
			else if(fViewportDisplayColumnIndex<RecycleDisplayColEndIndex){
				if(fViewportDisplayColumnEndIndex>RecycleDisplayColEndIndex){
					// |    |		PrevVisible
					//  |///|---|	NewVisible
					//       Sec1

					// Sec1
					ViewGridDisplaySetupRows(false,RecycleDisplayRowIndex,RecycleDisplayRowEndIndex,RecycleDisplayColEndIndex,fViewportDisplayColumnEndIndex);
				}
				else{
					// |    |		PrevVisible
					//  |//|		NewVisible

					// no new columns
				}
			}
			else{
				// |   |		PrevVisible
				//       |---|	NewVisible
				ViewGridDisplaySetupRows(false,RecycleDisplayRowIndex,RecycleDisplayRowEndIndex,fViewportDisplayColumnIndex,fViewportDisplayColumnEndIndex);
			}
		}
	
	}

	auto PrevInUseRowSet=cnVar::MoveCast(fInUseRowSet);
	auto PrevInUseCellSet=cnVar::MoveCast(fInUseCellSet);

	for(auto RowPair : fRowMap){
		bool KeepRow=RowPair.Value->IsVisible();
		if(KeepRow==false){
			ufInt32 RowIndex=RowPair.Value->fRowIndex;
			if(RowIndex>=fViewportDisplayRowIndex && RowIndex<fViewportDisplayRowEndIndex){
				KeepRow=true;
			}
		}
		if(KeepRow){
			fInUseRowSet.Insert(RowPair.Value);
			for(auto CellPair : RowPair.Value->fCellMap){
				bool KeepColumn=CellPair.Value->IsVisible();
				if(KeepColumn==false){
					ufInt32 ColIndex=CellPair.Value->fColumnIndex;
					if(ColIndex>=fViewportDisplayColumnIndex && ColIndex<fViewportDisplayColumnEndIndex){
						KeepColumn=true;
					}
				}
				if(KeepColumn){
					fInUseCellSet.Insert(CellPair.Value);
				}
			}
		}
	}
	fInLayout=false;
}
//---------------------------------------------------------------------------
aClsRef<bcViewGrid::cRow> bcViewGrid::QueryRow(ufInt32 RowIndex)noexcept(true)
{
	aClsRef<bcViewGrid::cRow> RetRow;
	auto &CacheRow=fRowMap[RowIndex];
	if(CacheRow!=nullptr){
		RetRow=CacheRow;
	}
	else{
		RetRow=aClsCreate<cRow>();

		RetRow->SetupOwner(this,RowIndex);
		PositRow(RetRow);
		CacheRow=RetRow;
	}
	// store to visible row to keep it until next refresh
	fInUseRowSet.Insert(RetRow);
	return RetRow;
}
//---------------------------------------------------------------------------
void bcViewGrid::RowDeleted(cRow *Row)noexcept(true)
{
	auto Pair=fRowMap.GetPair(Row->fRowIndex);
	if(Pair!=nullptr){
		if(Pair->Value==Row)
			fRowMap.RemovePair(Pair);
	}
}
//---------------------------------------------------------------------------
void bcViewGrid::PositRow(cRow *Row)noexcept(true)
{
	cUIRectangle RowRect;

	if(fGridYColumn){
		RowRect.Top=fScrollLowwerLimit[fGridYColumn];
		RowRect.Bottom=fScrollUpperLimit[fGridYColumn];
	}
	else{
		RowRect.Left=fScrollLowwerLimit[fGridYColumn];
		RowRect.Right=fScrollUpperLimit[fGridYColumn];
	}

	ufInt32 DisplayRowIndex=ViewGridRowIndexToDisplayRowIndex(Row->fRowIndex);
	if(DisplayRowIndex==IndexNotFound)
		return;
	auto RowRange=ViewGridDisplayRowQueryRange(DisplayRowIndex);

	if(fGridXRow){
		RowRect.Left=RowRange.Begin;
		RowRect.Right=RowRange.End;
	}
	else{
		RowRect.Top=RowRange.Begin;
		RowRect.Bottom=RowRange.End;
	}
	
	ViewItemSetRectangle(Row,RowRect);
		
}
//---------------------------------------------------------------------------
void bcViewGrid::PositCell(cCell *Cell)noexcept(true)
{
	cUIRectangle CellRect;
	ufInt32 DisplayRowIndex=ViewGridRowIndexToDisplayRowIndex(Cell->fOwner->fRowIndex);
	if(DisplayRowIndex==IndexNotFound)
		return;
	auto RowRange=ViewGridDisplayRowQueryRange(DisplayRowIndex);

	if(fGridXRow){
		CellRect.Left=RowRange.Begin;
		CellRect.Right=RowRange.End;
	}
	else{
		CellRect.Top=RowRange.Begin;
		CellRect.Bottom=RowRange.End;
	}

	ufInt32 DisplayColIndex=ViewGridColumnIndexToDisplayColumnIndex(Cell->fColumnIndex);
	if(DisplayColIndex==IndexNotFound)
		return;

	auto CellRange=ViewGridDisplayColumnQueryRange(DisplayColIndex);

	if(fGridYColumn){
		CellRect.Top=CellRange.Begin;
		CellRect.Bottom=CellRange.End;
	}
	else{
		CellRect.Left=CellRange.Begin;
		CellRect.Right=CellRange.End;
	}

	ViewItemSetRectangle(Cell,CellRect);
}
//---------------------------------------------------------------------------
bcViewGrid::cRow::cRow()noexcept(true)
	: fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
bcViewGrid::cRow::~cRow()noexcept(true)
{
	for(auto CellPair : fCellMap){
		CellPair.Value->ClearOwner();
	}
	if(fOwner!=nullptr){
		fOwner->RowDeleted(this);
	}
}
//---------------------------------------------------------------------------
void bcViewGrid::cRow::SetupOwner(bcViewGrid *Owner,ufInt32 RowIndex)noexcept(true)
{
	fOwner=Owner;
	fRowIndex=RowIndex;
	fOwner->ViewItemAttach(this);
	for(auto CellPair : fCellMap){
		fOwner->ViewItemAttach(CellPair.Value);
	}
}
//---------------------------------------------------------------------------
void bcViewGrid::cRow::ClearOwner(void)noexcept(true)
{
	for(auto CellPair : fCellMap){
		fOwner->ViewItemDetach(CellPair.Value);
	}
	fOwner->ViewItemDetach(this);
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
iViewReference* bcViewGrid::cRow::GetViewReference(void)const noexcept(true)
{
	if(fOwner==nullptr)
		return nullptr;
	return fOwner->ViewItemGetViewReference(this);
}
//---------------------------------------------------------------------------
void bcViewGrid::cRow::SetViewReference(iViewReference *ViewRef)noexcept(true)
{
	if(fOwner==nullptr)
		return;
	fOwner->ViewItemSetViewReference(this,ViewRef);
}
//---------------------------------------------------------------------------
aClsRef<bcViewGrid::cCell> bcViewGrid::cRow::QueryCell(ufInt32 ColumnIndex)noexcept(true)
{
	if(fOwner==nullptr)
		return nullptr;

	aClsRef<bcViewGrid::cCell> RetCell;

	auto &CacheCell=fCellMap[ColumnIndex];
	if(CacheCell!=nullptr){
		RetCell=CacheCell;
	}
	else{
		RetCell=aClsCreate<cCell>();
		RetCell->SetupOwner(this,ColumnIndex);
		fOwner->PositCell(RetCell);
		CacheCell=RetCell;
	}


	// store to visible row to keep it until next refresh
	fOwner->fInUseCellSet.Insert(RetCell);

	return RetCell;
}
//---------------------------------------------------------------------------
void bcViewGrid::cRow::CellDeleted(cCell *Cell)noexcept(true)
{
	auto Pair=fCellMap.GetPair(Cell->fColumnIndex);
	if(Pair!=nullptr){
		if(Pair->Value==Cell)
			fCellMap.RemovePair(Pair);
	}
}
//---------------------------------------------------------------------------
bcViewGrid::cCell::cCell()noexcept(true)
	: fOwner(nullptr)
{
}
//---------------------------------------------------------------------------
bcViewGrid::cCell::~cCell()noexcept(true)
{
	if(fOwner!=nullptr){
		fOwner->CellDeleted(this);
	}
}
//---------------------------------------------------------------------------
void bcViewGrid::cCell::SetupOwner(cRow *Owner,ufInt32 ColumnIndex)noexcept(true)
{
	fOwner=Owner;
	fColumnIndex=ColumnIndex;
}
//---------------------------------------------------------------------------
void bcViewGrid::cCell::ClearOwner(void)noexcept(true)
{
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
iViewReference* bcViewGrid::cCell::GetViewReference(void)const noexcept(true)
{
	if(fOwner==nullptr)
		return nullptr;
	if(fOwner->fOwner==nullptr)
		return nullptr;
	return fOwner->fOwner->ViewItemGetViewReference(this);
}
//---------------------------------------------------------------------------
void bcViewGrid::cCell::SetViewReference(iViewReference *ViewRef)noexcept(true)
{
	if(fOwner==nullptr)
		return;
	if(fOwner->fOwner==nullptr)
		return;
	fOwner->fOwner->ViewItemSetViewReference(this,ViewRef);
}
//---------------------------------------------------------------------------
cControlList::cControlList()noexcept(true)
{
}
//---------------------------------------------------------------------------
cControlList::~cControlList()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
iControlListSource* cControlList::GetControlSource(void)const noexcept(true)
{
	return fControlSource;
}
//---------------------------------------------------------------------------
void cControlList::SetControlSource(iControlListSource *Source)noexcept(true)
{
	if(fControlSource!=nullptr){
		fControlSource->ControlListNotifySet.Remove(fListNotifyToken);
	}
	fControlSource=Source;
	if(fControlSource!=nullptr){
		fListNotifyToken=fControlSource->ControlListNotifySet.Insert([this]{
			ApplyControlSource();
			SetNeedUpdateListViews();
		});
		ApplyControlSource();
	}
	SetNeedUpdateListViews();
}
//---------------------------------------------------------------------------
void cControlList::ApplyControlSource(void)noexcept(true)
{
	if(ItemSize==0){
		fScrollLowwerLimit[!fListXItem]=0;
		fScrollUpperLimit[!fListXItem]=0;
		fScrollNoUpperLimit[!fListXItem]=false;
	}
	else{
		bool ItemCountCorrect=fControlSource->ListGetItemCount(fListItemCount);

		fScrollLowwerLimit[!fListXItem]=0;
		fScrollUpperLimit[!fListXItem]=ItemSize*fListItemCount+ItemBorder*(fListItemCount+1);
		fScrollNoUpperLimit[!fListXItem]=!ItemCountCorrect;
	}
}
//---------------------------------------------------------------------------
void cControlList::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	bcViewList::ScrollContentUpdateContentLayout();

	if(fControlSource!=nullptr){
		ufInt32 ItemCount;
		bool ItemCountCorrect=fControlSource->ListGetItemCount(ItemCount);

		if((ItemCountCorrect && fListItemCount!=ItemCount) || fListItemCount<ItemCount){
			fListItemCount=ItemCount;

			fScrollLowwerLimit[!fListXItem]=0;
			fScrollUpperLimit[!fListXItem]=ItemSize*fListItemCount+ItemBorder*(fListItemCount+1);
			fScrollNoUpperLimit[!fListXItem]=!ItemCountCorrect;
		}

	}
}
//---------------------------------------------------------------------------
cUIRange cControlList::ViewListDisplayQueryRange(ufInt32 DisplayIndex)noexcept(true)
{
	cUIRange Range;
	Range.Begin=DisplayIndex*(ItemSize+ItemBorder)+ItemBorder;
	Range.End=Range.Begin+ItemSize;
	return Range;
}
//---------------------------------------------------------------------------
bool cControlList::ViewListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)noexcept(true)
{
	if(Position<=1){
		DisplayIndex=0;
		return false;
	}

	Float32 ItemTotalSize=ItemSize+ItemBorder;
	if(ItemTotalSize==0){
		DisplayIndex=0;
		return false;
	}

	ufInt32 ItemTotalSizeInt=static_cast<ufInt32>(ItemTotalSize);

	ufInt32 Pos=static_cast<ufInt32>(Position-ItemBorder);
	DisplayIndex=static_cast<uIntn>(Pos/ItemTotalSizeInt);
	return Pos%ItemTotalSizeInt!=0;
}
//---------------------------------------------------------------------------
void cControlList::ViewListDisplaySetupItems(ufInt32 ItemIndex,ufInt32 ItemEndIndex)noexcept(true)
{
	if(fListItemCount<ItemEndIndex){
		fListItemCount=ItemEndIndex;
		// update row range
		fScrollUpperLimit[!fListXItem]=ItemSize*fListItemCount+ItemBorder*(fListItemCount+1);
	}

	if(fControlSource==nullptr)
		return;

	fControlSource->ListSetupItems(ItemIndex,ItemEndIndex-ItemIndex);

	for(ufInt32 i=ItemIndex;i<ItemEndIndex;i++){
		auto Item=QueryItem(i);
			
		auto ViewReference=fControlSource->ListItemMake(this,i);
		Item->SetViewReference(ViewReference);
		
	}

	fControlSource->ListClearItems();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cControlGrid::cControlGrid()noexcept(true)
{
	fControlSource=nullptr;
}
//---------------------------------------------------------------------------
cControlGrid::~cControlGrid()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
iControlGridSource* cControlGrid::GetControlSource(void)const noexcept(true)
{
	return fControlSource;
}
//---------------------------------------------------------------------------
void cControlGrid::SetControlSource(iControlGridSource *Source)noexcept(true)
{
	if(fControlSource!=nullptr){
		fControlSource->ControlGridNotifySet.Remove(fGridNotifyToken);
	}
	fControlSource=Source;
	if(fControlSource!=nullptr){
		fGridNotifyToken=fControlSource->ControlGridNotifySet.Insert([this]{
			ApplyControlSource();
			SetNeedUpdateGridViews();
		});
		ApplyControlSource();
	}
	SetNeedUpdateGridViews();
}
//---------------------------------------------------------------------------
void cControlGrid::ApplyControlSource(void)noexcept(true)
{
	auto ColCount=fControlSource->GridColumnCount();
		
	fGridColumns.Clear();
	fGridColumns.SetCapacity(ColCount);
	cColumnItem ColItem;
	Float32 ItemPos=0;
	for(ufInt16 ci=0;ci<ColCount;ci++){
		ItemPos+=fControlSource->GridColumnGetDistance(ci);
		ColItem.Range.Begin=ItemPos;
		ItemPos+=fControlSource->GridColumnGetSize(ci);
		ColItem.Range.End=ItemPos;
		fGridColumns.AppendMake(ColItem);
	}
	ItemPos+=fControlSource->GridColumnGetDistance(ColCount);

	fScrollLowwerLimit[fGridYColumn]=0;
	fScrollUpperLimit[fGridYColumn]=ItemPos;
	fScrollNoUpperLimit[fGridYColumn]=false;


	if(RowSize==0){
		fScrollLowwerLimit[!fGridXRow]=0;
		fScrollUpperLimit[!fGridXRow]=0;
		fScrollNoUpperLimit[!fGridXRow]=false;
	}
	else{
		bool RowCountCorrect=fControlSource->GridGetRowCount(fGridRowCount);

		fScrollLowwerLimit[!fGridXRow]=0;
		fScrollUpperLimit[!fGridXRow]=RowSize*fGridRowCount+RowBorder*(fGridRowCount+1);
		fScrollNoUpperLimit[!fGridXRow]=!RowCountCorrect;
	}
}
//---------------------------------------------------------------------------
void cControlGrid::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	bcViewGrid::ScrollContentUpdateContentLayout();

	if(fControlSource!=nullptr){
		ufInt32 RowCount;
		bool RowCountCorrect=fControlSource->GridGetRowCount(RowCount);

		if((RowCountCorrect && fGridRowCount!=RowCount) || fGridRowCount<RowCount){
			fGridRowCount=RowCount;

			fScrollLowwerLimit[!fGridXRow]=0;
			fScrollUpperLimit[!fGridXRow]=RowSize*fGridRowCount+RowBorder*(fGridRowCount+1);
			fScrollNoUpperLimit[!fGridXRow]=!RowCountCorrect;
		}

	}
}
//---------------------------------------------------------------------------
cUIRange cControlGrid::ViewGridDisplayRowQueryRange(ufInt32 DisplayRowIndex)noexcept(true)
{
	if(RowSize==0)
		return UIRangeZero;
	cUIRange Range;
	Range.Begin=DisplayRowIndex*(RowSize+RowBorder)+RowBorder;
	Range.End=Range.Begin+RowSize;
	return Range;
}
//---------------------------------------------------------------------------
cUIRange cControlGrid::ViewGridDisplayColumnQueryRange(ufInt32 ColumnIndex)noexcept(true)
{
	if(ColumnIndex>=fGridColumns.GetCount())
		return UIRangeZero;

	auto &Col=fGridColumns[ColumnIndex];
	return Col.Range;
}
//---------------------------------------------------------------------------
bool cControlGrid::ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &DisplayRowIndex)noexcept(true)
{
	if(RowSize==0 || RowPosition<=1){
		DisplayRowIndex=0;
		return false;
	}

	ufInt32 Pos=static_cast<ufInt32>(RowPosition);
	DisplayRowIndex=Pos/static_cast<ufInt32>(RowSize+RowBorder);
	return Pos%static_cast<ufInt32>(RowSize+RowBorder)!=0;
}
//---------------------------------------------------------------------------
bool cControlGrid::ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &ColumnIndex)noexcept(true)
{
	if(ColumnPosition<=1){
		ColumnIndex=0;
		return false;
	}
	uIntn Index;
	bool Found=cnMemory::ViewBinarySearch(Index,fGridColumns.GetItems(),fGridColumns.GetCount(),[ColumnPosition](const cColumnItem &Item){
		if(ColumnPosition<Item.Range.Begin)
			return 1;
		if(ColumnPosition<Item.Range.End)
			return 0;
		return -1;
	});
	ColumnIndex=static_cast<ufInt32>(Index);
	return Found;
}
//---------------------------------------------------------------------------
void cControlGrid::ViewGridDisplaySetupRows(bool,ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)noexcept(true)
{
	if(fGridRowCount<RowEndIndex){
		fGridRowCount=RowEndIndex;
		// update row range
		fScrollUpperLimit[!fGridXRow]=RowSize*fGridRowCount+RowBorder*(fGridRowCount+1);
	}

	if(fControlSource==nullptr)
		return;

	fControlSource->GridSetupRows(RowIndex,RowEndIndex-RowIndex);

	for(ufInt32 r=RowIndex;r<RowEndIndex;r++){
		auto Row=QueryRow(r);

		for(ufInt16 c=ColIndex;c<ColEndIndex;c++){
			auto Cell=Row->QueryCell(c);
	
			auto ViewReference=fControlSource->GridRowMakeCell(this,r,c);
			Cell->SetViewReference(ViewReference);
		}
		
	}

	fControlSource->GridClearRows();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cControlGridList::cControlGridList()noexcept(true)
	: fColumnCount(1)
{
	fControlSource=nullptr;
}
//---------------------------------------------------------------------------
cControlGridList::~cControlGridList()noexcept(true)
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
iControlListSource* cControlGridList::GetControlSource(void)const noexcept(true)
{
	return fControlSource;
}
//---------------------------------------------------------------------------
void cControlGridList::SetControlSource(iControlListSource *Source)noexcept(true)
{
	if(fControlSource!=nullptr){
		fControlSource->ControlListNotifySet.Remove(fListNotifyToken);
	}
	fControlSource=Source;
	if(fControlSource!=nullptr){
		fListNotifyToken=fControlSource->ControlListNotifySet.Insert([this]{
			ApplyControlSource();
			SetNeedUpdateGridViews();
		});
		ApplyControlSource();
	}
	SetNeedUpdateGridViews();
}
//---------------------------------------------------------------------------
void cControlGridList::ApplyControlSource(void)noexcept(true)
{
	if(RowSize==0){
		fScrollLowwerLimit[!fGridXRow]=0;
		fScrollUpperLimit[!fGridXRow]=0;
		fScrollNoUpperLimit[!fGridXRow]=false;
	}
	else{
		bool ItemCountCorrect=fControlSource->ListGetItemCount(fListItemCount);

		ufInt32 RowCount=fListItemCount/fColumnCount;

		fScrollLowwerLimit[!fGridXRow]=0;
		fScrollUpperLimit[!fGridXRow]=RowSize*RowCount+RowBorder*(RowCount+1);
		fScrollNoUpperLimit[!fGridXRow]=!ItemCountCorrect;
	}
}
//---------------------------------------------------------------------------
ufInt32 cControlGridList::GetColumnCount(void)const noexcept(true)
{
	return fColumnCount;
}
//---------------------------------------------------------------------------
void cControlGridList::RectangleChanged(bool Moved,bool Sized)noexcept(true)
{
	bcViewGrid::RectangleChanged(Moved,Sized);

	if(Sized){
		// set list width
		ufInt16 PrevColumnCount=fColumnCount;
		Float32 MinWidth=ColumnSize+ColumnBorder*2;
		if(fScrollViewportSize[fGridYColumn]<=MinWidth){
			fScrollUpperLimit[fGridYColumn]=MinWidth;

			fColumnCount=1;
		}
		else{
			Float32 ColumnUnitSize=ColumnSize+ColumnBorder;
			fColumnCount=static_cast<uIntn>((fScrollViewportSize[fGridYColumn]-ColumnBorder)/ColumnUnitSize);
	
			fScrollUpperLimit[fGridYColumn]=fColumnCount*ColumnUnitSize+ColumnBorder;
		}

		if(fColumnCount!=PrevColumnCount){
			ufInt32 RowCount=fListItemCount/fColumnCount;

			fScrollLowwerLimit[!fGridXRow]=0;
			fScrollUpperLimit[!fGridXRow]=RowSize*RowCount+RowBorder*(RowCount+1);
		}
	}
}
//---------------------------------------------------------------------------
cUIRange cControlGridList::ViewGridDisplayRowQueryRange(ufInt32 DisplayRowIndex)noexcept(true)
{
	cUIRange Range;
	Range.Begin=DisplayRowIndex*(RowSize+RowBorder)+RowBorder;
	Range.End=Range.Begin+RowSize;
	return Range;
}
//---------------------------------------------------------------------------
cUIRange cControlGridList::ViewGridDisplayColumnQueryRange(ufInt32 DisplayColumnIndex)noexcept(true)
{
	cUIRange Range;
	Range.Begin=DisplayColumnIndex*(ColumnSize+ColumnBorder)+ColumnBorder;
	Range.End=Range.Begin+ColumnSize;
	return Range;
}
//---------------------------------------------------------------------------
bool cControlGridList::ViewGridDisplayRowFindAt(Float32 RowPosition,ufInt32 &DisplayRowIndex)noexcept(true)
{
	if(RowPosition<=1){
		DisplayRowIndex=0;
		return false;
	}

	ufInt32 Pos=static_cast<ufInt32>(RowPosition);
	DisplayRowIndex=static_cast<uIntn>(Pos/(RowSize+1));
	return Pos%static_cast<ufInt32>(RowSize+1)!=0;
}
//---------------------------------------------------------------------------
bool cControlGridList::ViewGridDisplayColumnFindAt(Float32 ColumnPosition,ufInt32 &DisplayColumnIndex)noexcept(true)
{
	if(ColumnPosition<=1){
		DisplayColumnIndex=0;
		return false;
	}

	ufInt32 Pos=static_cast<ufInt32>(ColumnPosition);
	DisplayColumnIndex=static_cast<uIntn>(Pos/(ColumnSize+1));
	return Pos%static_cast<ufInt32>(ColumnSize+1)!=0;
}
//---------------------------------------------------------------------------
void cControlGridList::ScrollContentUpdateContentLayout(void)noexcept(true)
{
	bcViewGrid::ScrollContentUpdateContentLayout();

	if(fControlSource!=nullptr){
		ufInt32 ItemCount;
		bool ItemCountCorrect=fControlSource->ListGetItemCount(ItemCount);

		if((ItemCountCorrect && fListItemCount!=ItemCount) || fListItemCount<ItemCount){
			fListItemCount=ItemCount;

			ufInt32 RowCount=fListItemCount/fColumnCount;

			fScrollLowwerLimit[!fGridXRow]=0;
			fScrollUpperLimit[!fGridXRow]=RowSize*RowCount+RowBorder*(RowCount+1);
			fScrollNoUpperLimit[!fGridXRow]=!ItemCountCorrect;
		}

	}
}
//---------------------------------------------------------------------------
void cControlGridList::ViewGridDisplaySetupRows(bool,ufInt32 RowIndex,ufInt32 RowEndIndex,ufInt32 ColIndex,ufInt32 ColEndIndex)noexcept(true)
{
	if(fControlSource==nullptr)
		return;
	if(ColIndex==0 && ColEndIndex>=fColumnCount){
		ufInt32 ItemIndex=RowIndex*fColumnCount;
		ufInt32 ItemEndIndex=RowEndIndex*fColumnCount;

		fControlSource->ListSetupItems(ItemIndex,ItemEndIndex-ItemIndex);

		ufInt32 i=ItemIndex;
		for(ufInt32 r=RowIndex;r<RowEndIndex;r++){
			auto Row=QueryRow(r);
			
			for(ufInt32 c=ColIndex;c<ColEndIndex;c++){
				auto Cell=Row->QueryCell(c);
			
				auto ViewReference=fControlSource->ListItemMake(this,i);
				Cell->SetViewReference(ViewReference);

				i++;
			}
		
		}

		fControlSource->ListClearItems();
	}
	else{
		for(ufInt32 r=RowIndex;r<RowEndIndex;r++){
			ufInt32 RowItemIndex=r*fColumnCount;
			ufInt32 ItemIndex=RowItemIndex+ColIndex;
			ufInt32 ItemEndIndex=RowItemIndex+ColEndIndex;

			fControlSource->ListSetupItems(ItemIndex,ItemEndIndex-ItemIndex);

			auto Row=QueryRow(r);
			
			ufInt32 i=ItemIndex;
			for(ufInt32 c=ColIndex;c<ColEndIndex;c++){
				auto Cell=Row->QueryCell(c);
			
				auto ViewReference=fControlSource->ListItemMake(this,i);
				Cell->SetViewReference(ViewReference);

				i++;
			}
		
			fControlSource->ListClearItems();
		}
	}
}
//---------------------------------------------------------------------------
