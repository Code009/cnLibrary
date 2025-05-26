#include "GridContainer.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
scItemGrid::scItemGrid()noexcept(true)
	: fVisibleRowIndex(0)
	, fVisibleColumnIndex(0)
	, fDataCache(rCreate< cUIVisualDataCache<cGridData> >(cnSystem::CurrentUIThread::GetUIThread(),static_cast<cUIVisualDataCache<cGridData>::iDataProvider*>(this)))
{
}
//---------------------------------------------------------------------------
scItemGrid::~scItemGrid()noexcept(true)
{
	for(auto RowPair : fRowMap){
		if(RowPair.Value!=nullptr){
			delete RowPair.Value;
		}
	}
	for(auto ColPair : fColMap){
		if(ColPair.Value!=nullptr){
			delete ColPair.Value;
		}
	}
	for(auto CellPair : fCellMap){
		if(CellPair.Value!=nullptr){
			delete CellPair.Value;
		}
	}
}
//---------------------------------------------------------------------------
scItemGrid::operator iVisualData<cGridData> *()const noexcept(true)
{
	return fDataCache;
}
//---------------------------------------------------------------------------
iScrollContentContainer* scItemGrid::GetContainer(void)noexcept(true)
{
	return fContainer;
}
//---------------------------------------------------------------------------
void scItemGrid::SetContainer(iScrollContentContainer *Container)noexcept(true)
{
	if(fContainer!=nullptr){
		fContainer->SetContent(nullptr);
	}
	fContainer=Container;
	if(fContainer!=nullptr){
		fContainer->SetContent(this);
	}
}
//---------------------------------------------------------------------------
Float32 scItemGrid::GetRowOffset(void)const noexcept(true)
{
	if(fContainer==nullptr)
		return 0;
	return fContainer->GetOffset().y;
}
//---------------------------------------------------------------------------
Float32 scItemGrid::GetColumnOffset(void)const noexcept(true)
{
	if(fContainer==nullptr)
		return 0;
	return fContainer->GetOffset().x;
}
//---------------------------------------------------------------------------
bool scItemGrid::SetGridOffset(Float32 RowOffset,Float32 ColumnOffset)noexcept(true)
{
	if(fContainer==nullptr)
		return false;

	return fContainer->SetOffset({ColumnOffset,RowOffset});
}
//---------------------------------------------------------------------------
iUIView* scItemGrid::GetView(void)const noexcept(true)
{
	if(fContainer==nullptr)
		return nullptr;
	return fContainer->GetScrollView();
}
//---------------------------------------------------------------------------
void scItemGrid::UpdateColumnCount(uIntn ColumnCount,bool IsEstimated)noexcept(true)
{
	if(fColumnCount==ColumnCount)
		return;
	if(IsEstimated && fColumnCount>ColumnCount){
		// do not shrink row count
		return;
	}
	fColumnCount=ColumnCount;

	if(fContainer==nullptr)
		return;

	if(fDefaultRowSize==0){
		fContainer->SetScrollLimitX({0,0},false,false);
	}
	else{
		fContainer->SetScrollLimitX({0,static_cast<Float32>(fDefaultColumnSize*fColumnCount+fDefaultColumnPadding*(fColumnCount+1))},false,IsEstimated);
	}

	fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
void scItemGrid::SetDefaultColumnSize(ufInt16 Size)noexcept(true)
{
	fDefaultColumnSize=Size;

	if(fContainer==nullptr)
		return;
	fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
void scItemGrid::SetDefaultColumnPadding(ufInt16 Size)noexcept(true)
{
	fDefaultColumnPadding=Size;

	if(fContainer==nullptr)
		return;
	fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
void scItemGrid::UpdateRowCount(uIntn RowCount,bool IsEstimated)noexcept(true)
{
	if(fRowCount==RowCount)
		return;
	if(IsEstimated && fRowCount>RowCount){
		// do not shrink row count
		return;
	}
	fRowCount=RowCount;

	if(fContainer==nullptr)
		return;

	if(fDefaultRowSize==0){
		fContainer->SetScrollLimitY({0,0},false,false);
	}
	else{
		fContainer->SetScrollLimitY({0,static_cast<Float32>(fDefaultRowSize*fRowCount+fDefaultRowPadding*(fRowCount+1))},false,IsEstimated);
	}

	fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
void scItemGrid::SetDefaultRowSize(ufInt16 Size)noexcept(true)
{
	fDefaultRowSize=Size;

	if(fContainer==nullptr)
		return;
	fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
void scItemGrid::SetDefaultRowPadding(ufInt16 Size)noexcept(true)
{
	fDefaultRowPadding=Size;

	if(fContainer==nullptr)
		return;
	fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
sfInt16 scItemGrid::GetRowSize(uIntn RowIndex)noexcept(true)
{
	auto RowRangePair=fRowRangeMap.GetPair(RowIndex);
	if(RowRangePair==nullptr)
		return -1;

	return RowRangePair->Value.Size;
}
//---------------------------------------------------------------------------
void scItemGrid::SetRowSize(uIntn RowIndex,sfInt16 Size)noexcept(true)
{
	auto &RowRange=fRowRangeMap[RowIndex];
	if(Size<=0)
		RowRange.Size=0;
	else
		RowRange.Size=Size;

	RowRange.Update=true;

	if(fContainer!=nullptr && IsRowVisible(RowIndex)){
		fContainer->NotifyUpdateScroll();
	}
}
//---------------------------------------------------------------------------
void scItemGrid::ResetRowSize(uIntn RowIndex)noexcept(true)
{
	auto RowRangePair=fRowRangeMap.GetPair(RowIndex);
	if(RowRangePair==nullptr)
		return;

	if(RowRangePair->Value.Size>=0){
		RowRangePair->Value.Size=-1;
		RowRangePair->Value.Update=true;

		if(fContainer!=nullptr && IsRowVisible(RowIndex)){
			fContainer->NotifyUpdateScroll();
		}
	}
}
//---------------------------------------------------------------------------
sfInt16 scItemGrid::GetRowHeadPadding(uIntn RowIndex)noexcept(true)
{
	auto RowRangePair=fRowRangeMap.GetPair(RowIndex);
	if(RowRangePair==nullptr)
		return -1;
	return RowRangePair->Value.HeadPadding;
}
//---------------------------------------------------------------------------
void scItemGrid::SetRowHeadPadding(uIntn RowIndex,sfInt16 Size)noexcept(true)
{
	auto &RowRange=fRowRangeMap[RowIndex];
	if(Size<=0)
		RowRange.HeadPadding=0;
	else
		RowRange.HeadPadding=Size;
	RowRange.Update=true;

	if(fContainer!=nullptr && IsRowVisible(RowIndex)){
		fContainer->NotifyUpdateScroll();
	}
}
//---------------------------------------------------------------------------
void scItemGrid::ResetRowHeadPadding(uIntn RowIndex)noexcept(true)
{
	auto RowRangePair=fRowRangeMap.GetPair(RowIndex);
	if(RowRangePair==nullptr)
		return;

	if(RowRangePair->Value.HeadPadding>=0){
		RowRangePair->Value.HeadPadding=-1;
		RowRangePair->Value.Update=true;

		if(fContainer!=nullptr && IsRowVisible(RowIndex)){
			fContainer->NotifyUpdateScroll();
		}
	}
}
//---------------------------------------------------------------------------
sfInt16 scItemGrid::GetRowTailPadding(uIntn RowIndex)noexcept(true)
{
	auto RowRangePair=fRowRangeMap.GetPair(RowIndex);
	if(RowRangePair==nullptr)
		return -1;
	return RowRangePair->Value.TailPadding;
}
//---------------------------------------------------------------------------
void scItemGrid::SetRowTailPadding(uIntn RowIndex,sfInt16 Size)noexcept(true)
{
	auto &RowRange=fRowRangeMap[RowIndex];
	if(Size<=0)
		RowRange.TailPadding=0;
	else
		RowRange.TailPadding=Size;
	RowRange.Update=true;

	if(fContainer!=nullptr && IsRowVisible(RowIndex)){
		fContainer->NotifyUpdateScroll();
	}
}
//---------------------------------------------------------------------------
void scItemGrid::ResetRowTailPadding(uIntn RowIndex)noexcept(true)
{
	auto RowRangePair=fRowRangeMap.GetPair(RowIndex);
	if(RowRangePair==nullptr)
		return;

	if(RowRangePair->Value.TailPadding>=0){
		RowRangePair->Value.TailPadding=-1;
		RowRangePair->Value.Update=true;

		if(fContainer!=nullptr && IsRowVisible(RowIndex)){
			fContainer->NotifyUpdateScroll();
		}
	}
}
//---------------------------------------------------------------------------
uIntn scItemGrid::GetVisibleRowBegin(void)noexcept(true)
{
	return fVisibleRowIndex;
}
//---------------------------------------------------------------------------
uIntn scItemGrid::GetVisibleRowEnd(void)noexcept(true)
{
	return fVisibleRowIndex+fVisibleRowRanges.GetCount();
}
//---------------------------------------------------------------------------
uIntn scItemGrid::GetVisibleColBegin(void)noexcept(true)
{
	return fVisibleColumnIndex;
}
//---------------------------------------------------------------------------
uIntn scItemGrid::GetVisibleColEnd(void)noexcept(true)
{
	return fVisibleColumnIndex+fVisibleColRanges.GetCount();
}
//---------------------------------------------------------------------------
cUIRange scItemGrid::CalculateRowRange(uIntn RowIndex)noexcept(true)
{
	// check if in stay
	auto RowPair=fRowMap.GetPair(RowIndex);
	if(RowPair!=nullptr){
		if(RowPair->Value->fStayPos.Begin!=RowPair->Value->fStayPos.End){
			// stay row
			return RowPair->Value->fStayPos;
		}
	}

	cUIRange Range;
	bool Exists;
	auto RowRangePair=fRowRangeMap.FindPairLowwer(RowIndex,Exists);
	if(RowRangePair==nullptr){
		Range.Begin=static_cast<Float32>(RowIndex*(fDefaultRowSize+fDefaultRowPadding)+fDefaultRowPadding);
		Range.End=Range.Begin+fDefaultRowSize;
		return Range;
	}
	if(Exists){
		Range.Begin=RowRangePair->Value.Begin;
		Range.End=RowRangePair->Value.End;
		return Range;
	}
	uIntn IndexOffset=RowIndex-RowRangePair->Key;
	Range.Begin=static_cast<Float32>(IndexOffset*(fDefaultRowSize+fDefaultRowPadding)+fDefaultRowPadding);
	Range.End=Range.Begin+fDefaultRowSize;
	Range.Begin+=RowRangePair->Value.End;
	return Range;
}
//---------------------------------------------------------------------------
cUIRange scItemGrid::CalculateColumnRange(uIntn ColumnIndex)noexcept(true)
{
	cUIRange Range;
	bool Exists;
	auto ColRangePair=fColRangeMap.FindPairLowwer(ColumnIndex,Exists);
	if(ColRangePair==nullptr){
		Range.Begin=static_cast<Float32>(ColumnIndex*(fDefaultColumnSize+fDefaultColumnPadding)+fDefaultColumnPadding);
		Range.End=Range.Begin+fDefaultColumnSize;
		return Range;
	}
	if(Exists){
		Range.Begin=ColRangePair->Value.Begin;
		Range.End=ColRangePair->Value.End;
		return Range;
	}
	uIntn IndexOffset=ColumnIndex-ColRangePair->Key;
	Range.Begin=static_cast<Float32>(IndexOffset*(fDefaultColumnSize+fDefaultColumnPadding)+fDefaultColumnPadding);
	Range.End=Range.Begin+fDefaultColumnSize;
	Range.Begin+=ColRangePair->Value.End;
	return Range;
}
//---------------------------------------------------------------------------
void scItemGrid::ScrollContentShow(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void scItemGrid::ScrollContentHide(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void scItemGrid::ScrollContentUpdate(void)noexcept(true)
{
	cnLib_ASSERT(fContainer!=nullptr);

	auto ScrollOffset=fContainer->GetOffset();
	auto ViewportSize=fContainer->GetViewportSize();

	// row visible range
	cUIRange RowVisibleRange;
	RowVisibleRange.Begin=ScrollOffset.y;
	RowVisibleRange.End=ScrollOffset.y+ViewportSize.y;

	// column visible range
	cUIRange ColVisibleRange;
	ColVisibleRange.Begin=ScrollOffset.x;
	ColVisibleRange.End=ScrollOffset.x+ViewportSize.x;

	{
		auto PrevVisibleList=cnVar::MoveCast(fVisibleItemList);
		fVisibleItemList=cnVar::MoveCast(fVisibleItemTempList);
		fVisibleItemTempList=cnVar::MoveCast(PrevVisibleList);
	}
	// mark invisible
	for(auto *VisibleItem : fVisibleItemTempList){
		VisibleItem->fVisible=false;
	}
	// mark stay rows invisible
	for(auto &StayRow : fRowStayList){
		StayRow->fStay=false;
		StayRow->fVisible=false;

		fVisibleItemTempList.AppendMake(StayRow);
	}
	// mark stay columnss invisible
	// mark stay cells invisible
	for(auto &StayCell : fCellStayList){
		StayCell->fStay=false;
		StayCell->fVisible=false;

		fVisibleItemTempList.AppendMake(StayCell);
	}

	fRowStayList.Clear();
	fCellStayList.Clear();

	// scan visible
	fVisibleRowIndex=ScanVisibleRange(RowVisibleRange,fDefaultRowSize,fDefaultRowPadding,fRowRangeMap,fVisibleRowRanges);
	fVisibleColumnIndex=ScanVisibleRange(ColVisibleRange,fDefaultColumnSize,fDefaultColumnPadding,fColRangeMap,fVisibleColRanges);

	uIntn VisibleRowEndIndex=fVisibleRowIndex+fVisibleRowRanges.GetCount();
	uIntn VisibleColEndIndex=fVisibleColumnIndex+fVisibleColRanges.GetCount();
	
	// scan stay item
	ScanStayItems<cRow>(RowVisibleRange,fDefaultRowSize,fDefaultRowPadding,fVisibleRowIndex,VisibleRowEndIndex,fVisibleRowRanges,fRowMap,fRowRangeMap,fRowStayList);
	ScanStayItems<cColumn>(ColVisibleRange,fDefaultColumnSize,fDefaultColumnPadding,fVisibleColumnIndex,VisibleColEndIndex,fVisibleColRanges,fColMap,fColRangeMap,fColStayList);

	for(auto *StayCol : fColStayList){
		StayCol->UpdateVisible();
	}
	// scan stay cells
	for(auto *StayRow : fRowStayList){
		StayRow->UpdateVisible();

		cGridIndex CellIndex;
		CellIndex.Row=StayRow->fRowIndex;
		CellIndex.Column=fVisibleColumnIndex;
		bool Exists;
		auto StayCellPair=fCellMap.FindPairUpper(CellIndex,Exists);
		if(StayCellPair!=nullptr){
			while(StayCellPair!=fCellMap.end()){
				if(StayCellPair->Key.Row!=CellIndex.Row)
					break;
				if(StayCellPair->Key.Column>=VisibleColEndIndex)
					break;
				auto StayCell=StayCellPair->Value;
				StayCell->fVisible=true;
				StayCell->UpdateVisible();

				StayCell->fStay=true;
				StayCell->fStayLeaving=StayRow->fStayLeaving;
				fCellStayList.AppendMake(StayCell);
				++StayCellPair;
			}

		}

		for(auto *StayCol : fColStayList){
			CellIndex.Column=StayCol->fColumnIndex;
			StayCellPair=fCellMap.GetPair(CellIndex);
			if(StayCellPair!=nullptr){
				auto StayCell=StayCellPair->Value;
				StayCell->fVisible=true;
				StayCell->UpdateVisible();

				StayCell->fStay=true;
				StayCell->fStayLeaving=StayRow->fStayLeaving || StayCol->fStayLeaving;
				fCellStayList.AppendMake(StayCell);
			}
		}

	}

	{	// mark visible

		bool Exists;
		auto VisibleRowPair=fRowMap.FindPairUpper(fVisibleRowIndex,Exists);
		if(VisibleRowPair!=nullptr){
			while(VisibleRowPair!=fRowMap.end()){
				if(VisibleRowPair->Key>=VisibleRowEndIndex){
					break;
				}
				auto VisibleItem=VisibleRowPair->Value;
				VisibleItem->fVisible=true;
				VisibleItem->UpdateVisible();

				fVisibleItemList.AppendMake(VisibleItem);
				++VisibleRowPair;
			}
		}

		auto VisibleColPair=fColMap.FindPairUpper(fVisibleColumnIndex,Exists);
		if(VisibleColPair!=nullptr){
			while(VisibleColPair!=fColMap.end()){
				if(VisibleColPair->Key>=VisibleColEndIndex){
					break;
				}
				auto VisibleItem=VisibleColPair->Value;
				VisibleItem->fVisible=true;
				VisibleItem->UpdateVisible();

				fVisibleItemList.AppendMake(VisibleItem);
				++VisibleColPair;
			}
		}

		cGridIndex CellIndex;
		CellIndex.Row=fVisibleRowIndex;
		while(CellIndex.Row<VisibleRowEndIndex){
			CellIndex.Column=fVisibleColumnIndex;
			auto VisibleCellPair=fCellMap.FindPairUpper(CellIndex,Exists);
			if(VisibleCellPair!=nullptr){
				while(VisibleCellPair!=fCellMap.end()){
					if(VisibleCellPair->Key.Row!=CellIndex.Row)
						break;
					if(VisibleCellPair->Key.Column>=VisibleColEndIndex)
						break;
					auto VisibleItem=VisibleCellPair->Value;
					VisibleItem->fVisible=true;
					VisibleItem->UpdateVisible();

					fVisibleItemList.AppendMake(VisibleItem);
					++VisibleCellPair;
				}

			}
			for(auto *StayCol : fColStayList){
				CellIndex.Column=StayCol->fColumnIndex;
				auto StayCellPair=fCellMap.GetPair(CellIndex);
				if(StayCellPair!=nullptr){
					auto StayCell=StayCellPair->Value;
					StayCell->fVisible=true;
					StayCell->UpdateVisible();

					StayCell->fStay=StayCol->fStay;
					StayCell->fStayLeaving=StayCol->fStayLeaving;
					fCellStayList.AppendMake(StayCell);
				}
			}
			CellIndex.Row++;
		}

	}
	// notify last visible item. may duplicate notify newly visible item
	for(auto *VisibleItem : fVisibleItemTempList){
		VisibleItem->UpdateVisible();
	}
	fVisibleItemTempList.Clear();

	// notify content update for visible items

	for(auto *StayRow : fRowStayList){
		StayRow->NotifyContentUpdate();
	}

	for(auto *StayCell : fCellStayList){
		StayCell->NotifyContentUpdate();
	}
	for(auto *VisibleItem : fVisibleItemList){
		VisibleItem->NotifyContentUpdate();
	}


#if 0

	// update visual data
	{
		uIntn RowCount=fViewportDisplayRowEndIndex-fVisibleRowIndex;
		uIntn ColCount=fViewportDisplayColumnEndIndex-fVisibleColumnIndex;
		cUIRange RowRange=RowVisibleRange;
		cUIRange ColRange=ColVisibleRange;

		RowRange.Begin-=fScrollOffset[1];
		RowRange.End-=fScrollOffset[1];

		ColRange.Begin-=fScrollOffset[0];
		ColRange.End-=fScrollOffset[0];

		GridLineSetupStart(ColCount,RowCount,ColRange,RowRange);

		for(uIntn r=0;r<RowCount;r++){
			RowRange=ViewGridDisplayRowQueryRange(r+fVisibleRowIndex);
			RowRange.Begin-=fScrollOffset[1];
			RowRange.End-=fScrollOffset[1];
			GridLineSetupRange(1,r,RowRange);
		}
		for(uIntn c=0;c<ColCount;c++){
			ColRange=ViewGridDisplayColumnQueryRange(c+fVisibleColumnIndex);
			ColRange.Begin-=fScrollOffset[0];
			ColRange.End-=fScrollOffset[0];
			GridLineSetupRange(0,c,ColRange);
}
		GridLineSetupFinish();
	}
#endif

	ScrollContentUpdated();
}
//---------------------------------------------------------------------------
cnDataStruct::cSeqMapIterator<uIntn,scItemGrid::cPositionRecord> scItemGrid::FindRangeLowwer(Float32 RowPosition,bool &Match,cnRTL::cSeqMap<uIntn,cPositionRecord> &RangeMap)noexcept(true)
{
	uIntn RowRangeCount=RangeMap.GetCount();
	auto Ranges=RangeMap.GetValues();
	cCompareRange Compareor={RowPosition};
	uIntn SearchResult;
	Match=cnMemory::ViewBinarySearch(SearchResult,Ranges,RowRangeCount,Compareor);
	if(Match==false && SearchResult!=0){
		SearchResult--;
	}
	return RangeMap.GetPairAt(SearchResult);
}
//---------------------------------------------------------------------------
uIntn scItemGrid::ScanVisibleRange(cUIRange VisibleRange,sfInt16 DefaultSize,sfInt16 DefaultPadding,cnRTL::cSeqMap<uIntn,cPositionRecord> &RangeMap,cnRTL::cSeqList<cUIRange> &VisibleRangeList)noexcept(true)
{
	VisibleRangeList.Clear();
	cUIRange CurRange;
	bool RowMatch;
	auto RangePair=FindRangeLowwer(VisibleRange.Begin,RowMatch,RangeMap);
	if(RangePair==nullptr){
		// no range item
		sfInt32 Pos=static_cast<sfInt32>(VisibleRange.Begin);
		sfInt32 EndPos=static_cast<sfInt32>(VisibleRange.End);
		uIntn VisibleIndex=VisibleRange.Begin/static_cast<ufInt32>(DefaultSize+DefaultPadding);
		Float32 LastRangeEnd=static_cast<Float32>(VisibleIndex*static_cast<ufInt32>(DefaultSize+DefaultPadding));
		uIntn CurIndex=VisibleIndex;

		do{
			CurRange.Begin=LastRangeEnd+DefaultPadding;
			CurRange.End=CurRange.Begin+DefaultSize;
			VisibleRangeList.AppendMake(CurRange);
			CurIndex++;
			LastRangeEnd=CurRange.End;
		}while(LastRangeEnd<VisibleRange.End);

		return VisibleIndex;
	}
	uIntn VisibleIndex=RangePair->Key;
	uIntn ScanIndex;
	if(RowMatch==false){
		ufInt32 Pos=static_cast<ufInt32>(VisibleRange.Begin-RangePair->Value.End);
		uIntn IndexOffset=Pos/static_cast<ufInt32>(DefaultSize+DefaultPadding);
		VisibleIndex+=IndexOffset;
		CurRange.Begin=RangePair->Value.End+static_cast<Float32>(IndexOffset*static_cast<ufInt32>(DefaultSize+DefaultPadding));
		ScanIndex=VisibleIndex;
	}
	else{
		CurRange.Begin=RangePair->Value.Begin;
		CurRange.End=RangePair->Value.End;
		VisibleRangeList.AppendMake(CurRange);
		if(CurRange.End>=VisibleRange.End){
			// end of visible range
			return VisibleIndex;
		}
		ScanIndex=VisibleIndex+1;
		CurRange.Begin=CurRange.End;
	}
	Float32 LastRangeEnd=RangePair->Value.End;
	++RangePair;
	// scan down the row
	do{

		while(ScanIndex<RangePair->Key){
			CurRange.End=CurRange.Begin+DefaultSize;
			LastRangeEnd=CurRange.End;
			VisibleRangeList.AppendMake(CurRange);
			CurRange.Begin+=DefaultPadding;
			if(CurRange.Begin>=VisibleRange.End){
				// end of visible range
				return VisibleIndex;
			}
			ScanIndex++;
		}
		// update row range
		if(RangePair->Value.Update){
			RangePair->Value.Update=false;
			RangePair->Value.LastRecordEnd=LastRangeEnd;
			RangePair->Value.Begin=LastRangeEnd+DefaultPadding;
			RangePair->Value.End=RangePair->Value.Begin+1;
		}
		CurRange.Begin=RangePair->Value.Begin;
		CurRange.End=RangePair->Value.End;
		VisibleRangeList.AppendMake(CurRange);
		ScanIndex++;
		++RangePair;

		if(CurRange.End>=VisibleRange.End){
			// end of visible range
			return VisibleIndex;
		}
		CurRange.Begin=CurRange.End;
	}while(RangePair!=RangeMap.end());
	return VisibleIndex;
}
//---------------------------------------------------------------------------
template<class TItem>
void scItemGrid::ScanStayItems(cUIRange VisibleRange,sfInt16 DefaultSize,sfInt16 DefaultPadding,uIntn VisibleIndex,uIntn VisibleEndIndex,const cnRTL::cSeqList<cUIRange> &VisibleRanges,const cnRTL::cSeqMap<uIntn,TItem*> &ItemMap,const cnRTL::cSeqMap<uIntn,cPositionRecord> &ItemRangeMap,cnRTL::cSeqList<TItem*> &StayList)noexcept(true)
{
	cnRTL::cSeqList< cStayCalculateState<TItem> > StayCalcList;

	StayList.Clear();

	bool Exists;
	auto ItemPair=ItemMap.FindPairLowwer(VisibleIndex,Exists);
	if(ItemPair!=nullptr){
		// top stay
		for(;;){
			cStayCalculateState<TItem> State;
			auto Item=ItemPair->Value;
			if(Item->fStayBeginIndex!=IndexNotFound && Item->fStayEndIndex>=VisibleIndex){
				Item->fVisible=true;
				if(Item->fStayEndIndex>=VisibleEndIndex){
					State.Limit=VisibleRange.End;
				}
				else{
					State.Limit=VisibleRanges[Item->fStayEndIndex-VisibleIndex].End;
				}
				auto ItemRangePair=ItemRangeMap.GetPair(ItemPair->Key);
				State.Size=DefaultSize;
				if(ItemRangePair!=nullptr){
					if(ItemRangePair->Value.Size>=0){
						State.Size=ItemRangePair->Value.Size;
					}
				}
				State.Limit-=State.Size;

				State.Item=ItemPair->Value;
				StayCalcList.AppendMake(State);
			}

			if(ItemPair==ItemMap.begin())
				break;
			--ItemPair;
		}
		Float32 StayPos=VisibleRange.Begin;
		for(uIntn i=StayCalcList.GetCount();i!=0;){
			i--;

			auto &StayItem=StayCalcList[i];
			StayItem.Item->fStay=true;
			StayList.AppendMake(StayItem.Item);

			if(StayPos<StayItem.Limit){
				// normal stay
				StayItem.Item->fStayLeaving=false;
				StayItem.Item->fStayPos.Begin=StayPos;
				StayPos+=StayItem.Size;
				StayItem.Item->fStayPos.End=StayPos;
				StayPos+=DefaultPadding;
			}
			else{
				// leaving stay
				StayItem.Item->fStayLeaving=true;
				Float32 StayEnd=StayItem.Limit+StayItem.Size;
				StayItem.Item->fStayPos.Begin=StayItem.Limit;
				StayItem.Item->fStayPos.End=StayEnd;
				StayPos+=DefaultPadding;
				if(StayPos<StayEnd){
					StayPos=StayEnd;
				}
			}
		}
		StayCalcList.Clear();
	}
	ItemPair=ItemMap.FindPairUpper(VisibleEndIndex-1,Exists);
	if(ItemPair!=nullptr){
		// bottom stay
		while(ItemPair!=ItemMap.end()){
			cStayCalculateState<TItem> State;
			auto Item=ItemPair->Value;
			if(Item->fStayBeginIndex!=IndexNotFound && Item->fStayBeginIndex<VisibleEndIndex){
				Item->fVisible=true;
				if(Item->fStayBeginIndex<VisibleIndex){
					State.Limit=VisibleRange.Begin;
				}
				else{
					State.Limit=VisibleRanges[Item->fStayBeginIndex-VisibleIndex].Begin;
				}
				auto ItemRangePair=ItemRangeMap.GetPair(ItemPair->Key);
				State.Size=DefaultSize;
				if(ItemRangePair!=nullptr){
					if(ItemRangePair->Value.Size>=0){
						State.Size=ItemRangePair->Value.Size;
					}
				}
				State.Limit+=State.Size;

				State.Item=ItemPair->Value;
				StayCalcList.AppendMake(State);
			}
			++ItemPair;
		}

		Float32 StayPos=VisibleRange.End;
		for(uIntn i=StayCalcList.GetCount();i!=0;){
			i--;
			auto &StayItem=StayCalcList[i];
			StayItem.Item->fStay=true;
			if(StayPos>StayItem.Limit){
				// normal stay
				StayItem.Item->fStayLeaving=false;
				StayItem.Item->fStayPos.End=StayPos;
				StayPos-=StayItem.Size;
				StayItem.Item->fStayPos.Begin=StayPos;
			}
			else{
				// leaving stay
				StayItem.Item->fStayLeaving=true;
				StayItem.Item->fStayPos.Begin=StayItem.Limit-StayItem.Size;
				StayItem.Item->fStayPos.End=StayItem.Limit;
				if(StayPos>StayItem.Limit)
					StayPos=StayItem.Limit;
			}
			StayPos-=DefaultPadding;
			StayList.AppendMake(StayItem.Item);
		}
		StayCalcList.Clear();
	}
}
//---------------------------------------------------------------------------
scItemGrid::cRow* scItemGrid::FindRow(uIntn RowIndex)noexcept(true)
{
	auto pItem=fRowMap.GetPair(RowIndex);
	if(pItem==nullptr)
		return nullptr;
	return pItem->Value;
}
//---------------------------------------------------------------------------
scItemGrid::cRow* scItemGrid::QueryRow(uIntn RowIndex)noexcept(true)
{
	cRow *Row;
	auto &RowItem=fRowMap[RowIndex];
	if(RowItem!=nullptr){
		Row=RowItem;
	}
	else{
		Row=new cRow;
		Row->fOwner=this;
		RowItem=Row;
		Row->fRowIndex=RowIndex;
		Row->fNotifiedVisible=false;
		Row->fStayBeginIndex=IndexNotFound;
		Row->fStayEndIndex=IndexNotFound;
		Row->fVisible=false;
		Row->fStay=false;
		if(fContainer!=nullptr){
			fContainer->NotifyUpdateScroll();
		}
	}
	return Row;
}
//---------------------------------------------------------------------------
bool scItemGrid::CloseRow(uIntn RowIndex)noexcept(true)
{
	auto pItem=fRowMap.GetPair(RowIndex);
	if(pItem==nullptr)
		return false;

	auto *Row=pItem->Value;
	if(Row!=nullptr){
		delete Row;
	}

	fRowMap.RemovePair(pItem);
	return true;
}
//---------------------------------------------------------------------------
scItemGrid::cRow* scItemGrid::MoveRow(uIntn RowIndex,uIntn NewRowIndex)noexcept(true)
{
	auto pItem=fRowMap.GetPair(RowIndex);
	if(pItem==nullptr)
		return nullptr;	// no item to move

	auto Row=pItem->Value;

	bool NewRowItemExists;
	auto pNewItem=fRowMap.InsertPair(NewRowIndex,NewRowItemExists);
	if(NewRowItemExists)
		return nullptr;	// have item already

	Row->fRowIndex=NewRowIndex;
	pNewItem->Value=Row;

	fRowMap.Remove(RowIndex);

	if(fContainer!=nullptr){
		fContainer->NotifyUpdateScroll();
	}
	return Row;
}
//---------------------------------------------------------------------------
scItemGrid::cColumn* scItemGrid::FindColumn(uIntn ColumnIndex)noexcept(true)
{
	auto pItem=fColMap.GetPair(ColumnIndex);
	if(pItem==nullptr)
		return nullptr;
	return pItem->Value;
}
//---------------------------------------------------------------------------
scItemGrid::cColumn* scItemGrid::QueryColumn(uIntn ColumnIndex)noexcept(true)
{
	cColumn *Col;
	auto &ColItem=fColMap[ColumnIndex];
	if(ColItem!=nullptr){
		Col=ColItem;
	}
	else{
		Col=new cColumn;
		Col->fOwner=this;
		ColItem=Col;
		Col->fNotifiedVisible=false;
		Col->fStayBeginIndex=IndexNotFound;
		Col->fStayEndIndex=IndexNotFound;
		Col->fVisible=false;
		Col->fStay=false;
		Col->fColumnIndex=ColumnIndex;
		if(fContainer!=nullptr){
			fContainer->NotifyUpdateScroll();
		}
	}
	return Col;
}
//---------------------------------------------------------------------------
bool scItemGrid::CloseColumn(uIntn ColumnIndex)noexcept(true)
{
	auto pItem=fColMap.GetPair(ColumnIndex);
	if(pItem==nullptr)
		return false;

	auto *Col=pItem->Value;
	if(Col!=nullptr){
		delete Col;
	}

	fColMap.RemovePair(pItem);
	return true;
}
//---------------------------------------------------------------------------
scItemGrid::cColumn* scItemGrid::MoveColumn(uIntn ColumnIndex,uIntn NewColumnIndex)noexcept(true)
{
	auto pItem=fColMap.GetPair(ColumnIndex);
	if(pItem==nullptr)
		return nullptr;	// no item to move

	auto Col=pItem->Value;

	bool NewColumnItemExists;
	auto pNewItem=fColMap.InsertPair(ColumnIndex,NewColumnItemExists);
	if(NewColumnItemExists)
		return nullptr;	// have item already

	Col->fColumnIndex=NewColumnIndex;
	pNewItem->Value=Col;

	fColMap.Remove(ColumnIndex);

	if(fContainer!=nullptr){
		fContainer->NotifyUpdateScroll();
	}
	return Col;
}
//---------------------------------------------------------------------------
scItemGrid::cCell* scItemGrid::FindCell(uIntn RowIndex,uIntn ColumnIndex)noexcept(true)
{
	cGridIndex CellIndex={RowIndex,ColumnIndex};
	auto pCellItem=fCellMap.GetPair(CellIndex);
	if(pCellItem==nullptr)
		return nullptr;
	return pCellItem->Value;
}
//---------------------------------------------------------------------------
scItemGrid::cCell* scItemGrid::QueryCell(uIntn RowIndex,uIntn ColumnIndex)noexcept(true)
{
	cGridIndex CellIndex={RowIndex,ColumnIndex};
	auto &CellItem=fCellMap[CellIndex];
	cCell *Cell;
	if(CellItem!=nullptr){
		Cell=CellItem;
	}
	else{
		Cell=new cCell;
		Cell->fOwner=this;
		CellItem=Cell;
		Cell->fNotifiedVisible=false;
		Cell->fVisible=false;
		Cell->fRowIndex=RowIndex;
		Cell->fStay=false;
		Cell->fColumnIndex=ColumnIndex;
		if(fContainer!=nullptr){
			fContainer->NotifyUpdateScroll();
		}
	}
	return Cell;
}
//---------------------------------------------------------------------------
bool scItemGrid::CloseCell(uIntn RowIndex,uIntn ColumnIndex)noexcept(true)
{
	cGridIndex CellIndex={RowIndex,ColumnIndex};
	auto pCellItem=fCellMap.GetPair(CellIndex);
	if(pCellItem==nullptr){
		return false;
	}

	auto *Cell=pCellItem->Value;
	if(Cell!=nullptr){
		delete Cell;
	}

	fCellMap.RemovePair(pCellItem);
	return true;
}
//---------------------------------------------------------------------------
scItemGrid::cCell* scItemGrid::MoveCell(uIntn RowIndex,uIntn ColumnIndex,uIntn NewRowIndex,uIntn NewColumnIndex)noexcept(true)
{
	cGridIndex CellIndex={RowIndex,ColumnIndex};
	auto pCellItem=fCellMap.GetPair(CellIndex);
	if(pCellItem!=nullptr){
		return nullptr;	// no item to move
	}
	auto *Cell=pCellItem->Value;

	cGridIndex NewCellIndex={NewRowIndex,NewColumnIndex};
	bool NewCellItemExists;
	auto pNewCellItem=fCellMap.InsertPair(CellIndex,NewCellItemExists);
	if(pNewCellItem!=nullptr){
		return nullptr;	// already have item
	}

	Cell->fRowIndex=NewRowIndex;
	Cell->fColumnIndex=NewColumnIndex;
	pNewCellItem->Value=Cell;

	fCellMap.Remove(CellIndex);

	if(fContainer!=nullptr){
		fContainer->NotifyUpdateScroll();
	}
	return Cell;

}
//---------------------------------------------------------------------------
sfInt8 scItemGrid::cCompareRange::operator ()(const cPositionRecord &Src)noexcept(true)
{
	if(Src.End<Position){
		return -1;
	}
	if(Src.Begin>Position){
		return 1;
	}
	return 0;
}
//---------------------------------------------------------------------------
const cGridData* scItemGrid::QueryData(rPtr<iReference> &DataReference)noexcept(true)
{
	auto DataRef=rCreate<cDataReference>();
	auto &Data=DataRef->Data;

	DataReference=DataRef;
	return &Data;
}
//---------------------------------------------------------------------------
uIntn scItemGrid::FindRowIndexAt(Float32 RowPosition)noexcept(true)
{
	uIntn RowIndex=0;
	FindRowIndexAt(RowPosition,RowIndex);
	return RowIndex;
}
//---------------------------------------------------------------------------
uIntn scItemGrid::FindColumnIndexAt(Float32 ColumnPosition)noexcept(true)
{
	uIntn ColIndex=0;
	FindColumnIndexAt(ColumnPosition,ColIndex);
	return ColIndex;
}
//---------------------------------------------------------------------------
bool scItemGrid::FindRowIndexAt(Float32 RowPosition,uIntn &RowIndex)noexcept(true)
{
	if(fDefaultRowSize==0 || RowPosition<=1){
		RowIndex=0;
		return false;
	}

	ufInt32 Pos=static_cast<ufInt32>(RowPosition);
	RowIndex=Pos/static_cast<ufInt32>(fDefaultRowSize+fDefaultRowPadding);
	return Pos%static_cast<ufInt32>(fDefaultRowSize+fDefaultRowPadding)!=0;
}
//---------------------------------------------------------------------------
bool scItemGrid::FindColumnIndexAt(Float32 ColumnPosition,uIntn &ColumnIndex)noexcept(true)
{
	if(fDefaultRowSize==0 || ColumnPosition<=1){
		ColumnIndex=0;
		return false;
	}

	ufInt32 Pos=static_cast<ufInt32>(ColumnPosition);
	ColumnIndex=Pos/static_cast<ufInt32>(fDefaultColumnSize+fDefaultColumnPadding);
	return Pos%static_cast<ufInt32>(fDefaultColumnSize+fDefaultColumnPadding)!=0;

	//if(ColumnPosition<=1){
	//	ColumnIndex=0;
	//	return false;
	//}
	//uIntn Index;
	//bool Found=cnMemory::ViewBinarySearch(Index,fGridColumns.GetItems(),fGridColumns.GetCount(),[ColumnPosition](const cColumnItem &Item){
	//	if(ColumnPosition<Item.Range.Begin)
	//		return 1;
	//	if(ColumnPosition<Item.Range.End)
	//		return 0;
	//	return -1;
	//	});
	//ColumnIndex=static_cast<ufInt32>(Index);
	//return Found;
}
//---------------------------------------------------------------------------
bool scItemGrid::IsRowVisible(uIntn RowIndex)const noexcept(true)
{
	if(RowIndex<fVisibleRowIndex)
		return false;

	if(RowIndex-fVisibleRowIndex>=fVisibleRowRanges.GetCount()){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool scItemGrid::IsColumnVisible(uIntn ColumnIndex)const noexcept(true)
{
	if(ColumnIndex<fVisibleColumnIndex)
		return false;

	if(ColumnIndex-fVisibleColumnIndex>=fVisibleColRanges.GetCount()){
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
sfInt8 scItemGrid::cGridIndex::Compare(const cGridIndex &Src)const noexcept(true)
{
	if(Row==Src.Row){
		return cnVar::DefaultCompare(Column,Src.Column);
	}
	else{
		sfInt8 lt=-static_cast<sfInt8>(Row<Src.Row);
		return 1|lt;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
iUIView* scItemGrid::bcItem::GetScrollView(void)noexcept(true)
{
	return fOwner->fContainer->GetScrollView();
}
//---------------------------------------------------------------------------
cUIPoint scItemGrid::bcItem::GetOffset(void)noexcept(true)
{
	return fOwner->fContainer->GetOffset();
}
//---------------------------------------------------------------------------
bool scItemGrid::bcItem::SetOffset(cUIPoint Offset)noexcept(true)
{
	return fOwner->fContainer->SetOffset(Offset);
}
//---------------------------------------------------------------------------
cUIRange scItemGrid::bcItem::GetScrollLimitX(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)
{
	return {0,0};
}
//---------------------------------------------------------------------------
void scItemGrid::bcItem::SetScrollLimitX(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)
{
}
//---------------------------------------------------------------------------
cUIRange scItemGrid::bcItem::GetScrollLimitY(bool &NoLowwerLimit,bool &NoUpperLimit)noexcept(true)
{
	return {0,0};
}
//---------------------------------------------------------------------------
void scItemGrid::bcItem::SetScrollLimitY(cUIRange OffsetRange,bool NoLowwerLimit,bool NoUpperLimit)noexcept(true)
{
}
//---------------------------------------------------------------------------
cUIPoint scItemGrid::bcItem::GetViewportSize(void)noexcept(true)
{
	return fOwner->fContainer->GetViewportSize();
}
//---------------------------------------------------------------------------
void scItemGrid::bcItem::NotifyUpdateScroll(void)noexcept(true)
{
	return fOwner->fContainer->NotifyUpdateScroll();
}
//---------------------------------------------------------------------------
bool scItemGrid::bcItem::IsRectangleInViewport(const cUIRectangle &Rect)noexcept(true)
{
	return fOwner->fContainer->IsRectangleInViewport(Rect);
}
//---------------------------------------------------------------------------
iScrollContent* scItemGrid::bcItem::GetContent(void)noexcept(true)
{
	return fContent;
}
//---------------------------------------------------------------------------
void scItemGrid::bcItem::SetContent(iScrollContent *Content)noexcept(true)
{
	if(fContent!=nullptr){
		if(fNotifiedVisible){
			fContent->ScrollContentHide();
			fNotifiedVisible=false;
		}
	}
	fContent=Content;
	if(fContent!=nullptr){
		if(fVisible){
			fNotifiedVisible=true;
			fContent->ScrollContentShow();
		}
	}
}
//---------------------------------------------------------------------------
void scItemGrid::bcItem::UpdateVisible(void)noexcept(true)
{
	if(fContent==nullptr)
		return;

	if(fNotifiedVisible==fVisible)
		return;

	fNotifiedVisible=fVisible;
	if(fNotifiedVisible){
		fContent->ScrollContentShow();
	}
	else{
		fContent->ScrollContentHide();
	}
}
//---------------------------------------------------------------------------
void scItemGrid::bcItem::NotifyContentUpdate(void)noexcept(true)
{
	if(fContent==nullptr)
		return;

	if(fNotifiedVisible){
		fContent->ScrollContentUpdate();
	}
}
//---------------------------------------------------------------------------
scItemGrid::ItemLayer scItemGrid::bcItem::GetLayer(void)const noexcept(true)
{
	if(fStay){
		return fStayLeaving?ItemLayer::Leave:ItemLayer::Stay;
	}
	return ItemLayer::Normal;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
scItemGrid::cRow::cRow()noexcept(true)
{
}
//---------------------------------------------------------------------------
scItemGrid::cRow::~cRow()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool scItemGrid::cRow::Close(void)noexcept(true)
{
	return fOwner->CloseRow(fRowIndex);
}
//---------------------------------------------------------------------------
uIntn scItemGrid::cRow::GetRowIndex(void)const noexcept(true)
{
	return fRowIndex;
}
//---------------------------------------------------------------------------
bool scItemGrid::cRow::SetRowIndex(uIntn NewRowIndex)noexcept(true)
{
	return nullptr!=fOwner->MoveRow(fRowIndex,NewRowIndex);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void scItemGrid::cRow::SetStayRange(uIntn Begin,uIntn End)noexcept(true)
{
	if(Begin>fRowIndex)
		Begin=fRowIndex;
	if(End<fRowIndex)
		End=fRowIndex;
	fStayBeginIndex=Begin;
	fStayEndIndex=End;
	if(fOwner->fContainer!=nullptr){
		fOwner->fContainer->NotifyUpdateScroll();
	}
}
//---------------------------------------------------------------------------
void scItemGrid::cRow::ResetStayRange(void)noexcept(true)
{
	fStayBeginIndex=IndexNotFound;
	fStayEndIndex=IndexNotFound;
	if(fOwner->fContainer!=nullptr){
		fOwner->fContainer->NotifyUpdateScroll();
	}
}
//---------------------------------------------------------------------------
cUIRect scItemGrid::cRow::CalculateLayoutRect(void)const noexcept(true)
{
	if(fOwner->fContainer==nullptr){
		return UIRectZero;
	}

	auto ScrollOffset=fOwner->fContainer->GetOffset();
	auto ViewportSize=fOwner->fContainer->GetViewportSize();

	auto RowRange=fOwner->CalculateRowRange(fRowIndex);

	cUIRect ItemRect;

	ItemRect.Pos.x=0;
	ItemRect.Pos.y=RowRange.Begin-ScrollOffset.y;
	ItemRect.Size.x=ViewportSize.x;
	ItemRect.Size.y=RowRange.End-RowRange.Begin;

	return ItemRect;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
scItemGrid::cColumn::cColumn()noexcept(true)
{
}
//---------------------------------------------------------------------------
scItemGrid::cColumn::~cColumn()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool scItemGrid::cColumn::Close(void)noexcept(true)
{
	return fOwner->CloseColumn(fColumnIndex);
}
//---------------------------------------------------------------------------
uIntn scItemGrid::cColumn::GetColumnIndex(void)const noexcept(true)
{
	return fColumnIndex;
}
//---------------------------------------------------------------------------
bool scItemGrid::cColumn::SetColumnIndex(uIntn NewColumnIndex)noexcept(true)
{
	return nullptr!=fOwner->MoveColumn(fColumnIndex,NewColumnIndex);
}
//---------------------------------------------------------------------------
cUIRect scItemGrid::cColumn::CalculateLayoutRect(void)const noexcept(true)
{
	if(fOwner->fContainer==nullptr){
		return UIRectZero;
	}

	auto ScrollOffset=fOwner->fContainer->GetOffset();
	auto ViewportSize=fOwner->fContainer->GetViewportSize();

	auto ColRange=fOwner->CalculateColumnRange(fColumnIndex);

	cUIRect ItemRect;

	ItemRect.Pos.x=ColRange.Begin-ScrollOffset.x;
	ItemRect.Pos.y=0;
	ItemRect.Size.x=ColRange.End-ColRange.Begin;
	ItemRect.Size.y=ViewportSize.y;

	return ItemRect;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
scItemGrid::cCell::cCell()noexcept(true)
{
}
//---------------------------------------------------------------------------
scItemGrid::cCell::~cCell()noexcept(true)
{
}
//---------------------------------------------------------------------------
bool scItemGrid::cCell::Close(void)noexcept(true)
{
	return fOwner->CloseCell(fRowIndex,fColumnIndex);
}
//---------------------------------------------------------------------------
uIntn scItemGrid::cCell::GetRowIndex(void)const noexcept(true)
{
	return fRowIndex;
}
//---------------------------------------------------------------------------
uIntn scItemGrid::cCell::GetColumnIndex(void)const noexcept(true)
{
	return fColumnIndex;
}
//---------------------------------------------------------------------------
bool scItemGrid::cCell::Move(uIntn NewRowIndex,uIntn NewColumnIndex)noexcept(true)
{
	return nullptr!=fOwner->MoveCell(fRowIndex,fColumnIndex,NewRowIndex,NewColumnIndex);
}
//---------------------------------------------------------------------------
cUIRect scItemGrid::cCell::CalculateLayoutRect(void)const noexcept(true)
{
	if(fOwner->fContainer==nullptr){
		return UIRectZero;
	}

	auto ScrollOffset=fOwner->fContainer->GetOffset();
	auto ViewportSize=fOwner->fContainer->GetViewportSize();

	auto RowRange=fOwner->CalculateRowRange(fRowIndex);
	auto ColRange=fOwner->CalculateColumnRange(fColumnIndex);

	cUIRect ItemRect;

	ItemRect.Pos.x=ColRange.Begin-ScrollOffset.x;
	ItemRect.Pos.y=RowRange.Begin-ScrollOffset.y;
	ItemRect.Size.x=ColRange.End-ColRange.Begin;
	ItemRect.Size.y=RowRange.End-RowRange.Begin;

	return ItemRect;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
blItemGridManager::blItemGridManager()noexcept(true)
	: fGrid(nullptr)
{
}
//---------------------------------------------------------------------------
blItemGridManager::~blItemGridManager()noexcept(true)
{
	SetGrid(nullptr);
}
//---------------------------------------------------------------------------
scItemGrid* blItemGridManager::GetGrid(scItemGrid *Grid)noexcept(true)
{
	return fGrid;
}
//---------------------------------------------------------------------------
void blItemGridManager::SetGrid(scItemGrid *Grid)noexcept(true)
{
	if(fGrid!=nullptr){
		fGrid->ScrollContentUpdated.Remove(fScrollContentUpdateToken);
		GridClear();
	}
	fGrid=Grid;
	if(fGrid!=nullptr){
		fScrollContentUpdateToken=fGrid->ScrollContentUpdated.Insert([this]()noexcept(true){
			GridUpdateData();
			});

		GridSetup();
	}
}
//---------------------------------------------------------------------------
void blItemGridManager::GridSetup(void)noexcept(true)
{
	GridUpdateData();
}
//---------------------------------------------------------------------------
void blItemGridManager::GridClear(void)noexcept(true)
{
}
//---------------------------------------------------------------------------
void blItemGridManager::GridUpdateData(void)noexcept(true)
{
	fView->SetArrangement();

	auto RowBegin=fGrid->GetVisibleRowBegin();
	auto RowEnd=fGrid->GetVisibleRowEnd();
	auto ColBegin=fGrid->GetVisibleColBegin();
	auto ColEnd=fGrid->GetVisibleColEnd();

	GridUpdate(RowBegin,RowEnd,ColBegin,ColEnd);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcItemGridRow::ScrollContentShow(void)noexcept(true)
{
	fView->SetVisible(true);
}
//---------------------------------------------------------------------------
void bcItemGridRow::ScrollContentHide(void)noexcept(true)
{
	fView->SetVisible(false);
}
//---------------------------------------------------------------------------
void bcItemGridRow::ScrollContentUpdate(void)noexcept(true)
{
	auto Layer=Row->GetLayer();
	switch(Layer){
	case scItemGrid::ItemLayer::Normal:
		fView->SetZPosition(0);
		break;
	case scItemGrid::ItemLayer::Leave:
		fView->SetZPosition(1);
		break;
	case scItemGrid::ItemLayer::Stay:
		fView->SetZPosition(2);
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcItemGridColumn::ScrollContentShow(void)noexcept(true)
{
	fView->SetVisible(true);
}
//---------------------------------------------------------------------------
void bcItemGridColumn::ScrollContentHide(void)noexcept(true)
{
	fView->SetVisible(false);
}
//---------------------------------------------------------------------------
void bcItemGridColumn::ScrollContentUpdate(void)noexcept(true)
{
	auto Layer=Column->GetLayer();
	switch(Layer){
	case scItemGrid::ItemLayer::Normal:
		fView->SetZPosition(0);
		break;
	case scItemGrid::ItemLayer::Leave:
		fView->SetZPosition(1);
		break;
	case scItemGrid::ItemLayer::Stay:
		fView->SetZPosition(2);
		break;
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void bcItemGridCell::ScrollContentShow(void)noexcept(true)
{
	fView->SetVisible(true);
}
//---------------------------------------------------------------------------
void bcItemGridCell::ScrollContentHide(void)noexcept(true)
{
	fView->SetVisible(false);
}
//---------------------------------------------------------------------------
void bcItemGridCell::ScrollContentUpdate(void)noexcept(true)
{
	auto Layer=Cell->GetLayer();
	switch(Layer){
	case scItemGrid::ItemLayer::Normal:
		fView->SetZPosition(0);
		break;
	case scItemGrid::ItemLayer::Leave:
		fView->SetZPosition(100);
		break;
	case scItemGrid::ItemLayer::Stay:
		fView->SetZPosition(200);
		break;
	}
}
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
