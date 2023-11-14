#include "TreeContainer.h"

//---------------------------------------------------------------------------
using namespace cnLibrary;
using namespace cnUI;

//---------------------------------------------------------------------------
rPtr<viControl> (*cnUI::gCreateDefaultTreeBranchVisual)(viTreeBranchData *Data)=nullptr;
rPtr<viControl> (*cnUI::gCreateDefaultTreeExpandButtonVisual)(viButtonData *Data)=nullptr;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcViewTreeContainer::bcViewTreeContainer()
	: fTreeXBranch(false)
	, fTreeBranchUpper(false)
	, fTreeLevelIndent(24)
{
}
//---------------------------------------------------------------------------
bcViewTreeContainer::~bcViewTreeContainer()
{
	SetView(nullptr);
	InvalidateData();
}
//---------------------------------------------------------------------------
bcViewTreeContainer::cTreeBranchInfo bcViewTreeContainer::TreeBranchInfo(void)
{
	cTreeBranchInfo BranchInfo;
	BranchInfo.BranchCount=fTreeBranchs.GetCount();
	BranchInfo.BranchDirection=GetTreeDirection();
	return BranchInfo;
}
//---------------------------------------------------------------------------
bcViewTreeContainer::cTreeBranchItem bcViewTreeContainer::TreeBranchItem(ufInt16 BranchIndex)
{
	cTreeBranchItem BranchItem;
	auto &CacheBranch=fTreeBranchs[BranchIndex];
	BranchItem.Rect=CacheBranch.Rect;
	BranchItem.NodeOffsets=CacheBranch.NodeOffsetList.GetItems();
	BranchItem.NodeCount=CacheBranch.NodeOffsetList.GetCount();
	return BranchItem;
}
//---------------------------------------------------------------------------
eDirection bcViewTreeContainer::GetTreeDirection(void)const
{
	return static_cast<eDirection>(fTreeBranchUpper)+(static_cast<eDirection>(fTreeXBranch)<<1);
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::SetTreeDirection(eDirection Direction)
{
	fTreeBranchUpper=(Direction&1)!=0;
	fTreeXBranch=((Direction>>1)&1)!=0;
}
//---------------------------------------------------------------------------
Float32 bcViewTreeContainer::GetTreeLevelIdent(void)const
{
	return fTreeLevelIndent;
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::SetTreeLevelIdent(Float32 LevelIdent)
{
	if(fTreeLevelIndent!=LevelIdent){
		fTreeLevelIndent=LevelIdent;
		//ReloadTreeBranchData();
		//fView->SetArrangement();
	}
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::ControlTreeBranchContentSetDefault(void)
{
	if(gCreateDefaultTreeBranchVisual!=nullptr){
		SetTreeBranchContent((*gCreateDefaultTreeBranchVisual)(this));
	}
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::SetTreeBranchContent(rPtr<viControl> Content)
{
	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetView(nullptr);
	}
	fTreeBranchContent=cnVar::MoveCast(Content);
	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetContentZPosition(ControlZOffset+TreeBranchContentZPosition);
		fTreeBranchContent->SetView(fView);
	}
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::UpdateControlZOffset(void)
{
	cScrollViewContainer::UpdateControlZOffset();

	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetContentZPosition(ControlZOffset+TreeBranchContentZPosition);
	}
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::ViewSetup(void)
{
	cScrollViewContainer::ViewSetup();

	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetView(fView);
	}
	else{
		ControlTreeBranchContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::ViewClear(void)
{
	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetView(nullptr);
	}
	
	cScrollViewContainer::ViewClear();
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::TreeBranchDataClear(void)
{
	fTreeBranchs.Clear();
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::TreeBranchDataBuildStart(cUIRange ContentRange)
{
	fBuildingContentRange=ContentRange;
	fBuildingLastNodeEndPos=fBuildingContentRange.Begin;
	fBuildingBranchs.Clear();
	fTreeBranchs.Clear();
}
//---------------------------------------------------------------------------
void bcViewTreeContainer::TreeBranchDataBuildBranch(BuildContext &BuildContext,cUIRange ItemRange,sfInt16 Level,bool IsLastNode)
{
	Float32 NodePos[2];
	NodePos[!BuildContext.GridXRow]=(ItemRange.Begin+ItemRange.End)/2;
	Float32 CurLevelIndent=Level*BuildContext.LevelIndent;
	NodePos[BuildContext.GridXRow]=CurLevelIndent+BuildContext.LevelIndent;
	
	if(Level<0){
		fBuildingLastNodeEndPos=ItemRange.End;
		return;
	}
	uIntn CurLevelIndex=Level+1;

	auto BuildingLevelCount=fBuildingBranchs.GetCount();

	if(CurLevelIndex>BuildingLevelCount){
		uIntn NewLevelIndex=BuildingLevelCount;
		fBuildingBranchs.SetCount(CurLevelIndex);

		do{
			auto &Branch=fBuildingBranchs[NewLevelIndex];
			Branch.LevelFinished=true;
			NewLevelIndex++;
			if(BuildContext.GridXRow){
				Branch.Rect.Top=CurLevelIndent;
				Branch.Rect.Bottom=CurLevelIndent+BuildContext.LevelIndent;
				Branch.Rect.Left=fBuildingLastNodeEndPos;
				Branch.Rect.Bottom=fBuildingLastNodeEndPos;
			}
			else{
				Branch.Rect.Left=CurLevelIndent;
				Branch.Rect.Right=CurLevelIndent+BuildContext.LevelIndent;
				Branch.Rect.Top=fBuildingLastNodeEndPos;
				Branch.Rect.Bottom=fBuildingLastNodeEndPos;
			}
		}while(NewLevelIndex<CurLevelIndex);
	}
	else if(CurLevelIndex<BuildingLevelCount){
		for(uIntn i=CurLevelIndex;i<BuildingLevelCount;i++){
			fTreeBranchs.AppendMake(cnVar::MoveCast(fBuildingBranchs[i]));
		}
		fBuildingBranchs.SetCount(CurLevelIndex);
	}
	
	fBuildingLastNodeEndPos=ItemRange.End;
	cnLib_ASSERT(Level<static_cast<sfInt16>(fBuildingBranchs.GetCount()));
	auto &ParentBranch=fBuildingBranchs[Level];
	Float32 &ParentBranchRangeEnd=BuildContext.GridXRow?ParentBranch.Rect.Right:ParentBranch.Rect.Bottom;
	ParentBranchRangeEnd=NodePos[!BuildContext.GridXRow];
	cUIPoint NodePoint={NodePos[0],NodePos[1]};
	ParentBranch.NodeOffsetList.AppendMake(NodePoint.x);
	ParentBranch.LevelFinished=IsLastNode;

}
//---------------------------------------------------------------------------
void bcViewTreeContainer::TreeBranchDataBuildFinish(BuildContext &BuildContext)
{
	Float32 cUIRectangle::*pBranchRangeEnd=BuildContext.GridXRow?&cUIRectangle::Right:&cUIRectangle::Bottom;
	for(auto &Branch : fBuildingBranchs){
		if(Branch.LevelFinished==false){
			Branch.Rect.*pBranchRangeEnd=fBuildingContentRange.End;
		}
	
		fTreeBranchs.AppendMake(cnVar::MoveCast(Branch));
	}
	for(auto &Branch : fTreeBranchs){
		Branch.Rect.Left+=BuildContext.TreePos.x;
		Branch.Rect.Right+=BuildContext.TreePos.x;
		Branch.Rect.Top+=BuildContext.TreePos.y;
		Branch.Rect.Bottom+=BuildContext.TreePos.y;

		for(auto &Node : Branch.NodeOffsetList){
			//Node.x+=BuildContext.TreePos.x;
			//Node.y+=BuildContext.TreePos.y;
		}
	}
}
//---------------------------------------------------------------------------
#if 0

void bcViewTreeContainer::ReloadTreeBranchData(void)
{
	auto NodeCount=VisibleRowGetCount();
	if(NodeCount==0){
		fTreeBranchData.Clear();
		return;
	}
	auto FirstNode=TreeNodeGetInfo(0);
	if(FirstNode==nullptr){
		fTreeBranchData.Clear();
		return;
	}

	cUIRange ContentRange;
	ContentRange.Begin=0;
	ContentRange.End=fScrollViewportSize[!fTreeHorizontal];
	fTreeBranchData.BuildStart(ContentRange);

	cControlTreeBranchData::BuildContext TreeBranchBuildContext;
	
	TreeBranchBuildContext.TreePos.x=ContentToView(0,false);
	TreeBranchBuildContext.TreePos.y=ContentToView(0,true);
	TreeBranchBuildContext.GridXRow=fTreeHorizontal;
	TreeBranchBuildContext.LevelIndent=fTreeLevelIndent;

	//Float32 LastNodePrimaryEnd=ContentRange.Begin;
	{
		// build head node list
		aCls<cTreeNodeInfo> *TopNode=FirstNode->Parent;
		cnRTL::cSeqList< aClsRef<cTreeNodeInfo> > Nodes;
		while(TopNode!=nullptr){

			Nodes.AppendMake(TopNode);
		
			TopNode=TopNode->Parent;
		}

		cUIRange TopRange={ContentRange.Begin,ContentRange.Begin};

		for(uIntn i=Nodes.GetCount();i>0;){
			i--;
			fTreeBranchData.BuildBranch(TreeBranchBuildContext,TopRange,Nodes[i]->Level,Nodes[i]->IsLastNode);
		}

	}
	fTreeBranchData.BuildBranch(TreeBranchBuildContext,VisibleRowGetRange(0),FirstNode->Level,FirstNode->IsLastNode);
	
	for(uIntn i=1;i<NodeCount;i++){
		auto NodeInfo=TreeNodeGetInfo(i);
		if(NodeInfo!=nullptr){
			fTreeBranchData.BuildBranch(TreeBranchBuildContext,VisibleRowGetRange(i),NodeInfo->Level,NodeInfo->IsLastNode);
		}
	}
	fTreeBranchData.BuildFinish(TreeBranchBuildContext);
}
#endif // 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cControlTreeBranchData::cControlTreeBranchData()
{
}
//---------------------------------------------------------------------------
cControlTreeBranchData::~cControlTreeBranchData()
{
}
//---------------------------------------------------------------------------
uIntn cControlTreeBranchData::GetBranchCount(void)const
{
	return fTreeBranchs.GetCount();
}
//---------------------------------------------------------------------------
viTreeBranchData::cTreeBranchItem cControlTreeBranchData::GetBranchItem(uIntn BranchIndex)const
{
	viTreeBranchData::cTreeBranchItem BranchItem;
	auto &CacheBranch=fTreeBranchs[BranchIndex];
	BranchItem.Rect=CacheBranch.Rect;
	//BranchItem.Nodes=CacheBranch.NodeList.GetItems();
	BranchItem.NodeCount=CacheBranch.NodeList.GetCount();
	return BranchItem;
}
//---------------------------------------------------------------------------
void cControlTreeBranchData::Clear(void)
{
	fTreeBranchs.Clear();
}
//---------------------------------------------------------------------------
void cControlTreeBranchData::BuildStart(cUIRange ContentRange)
{
	fBuildingContentRange=ContentRange;
	fBuildingLastNodeEndPos=fBuildingContentRange.Begin;
	fBuildingBranchs.Clear();
	fTreeBranchs.Clear();
}
//---------------------------------------------------------------------------
void cControlTreeBranchData::BuildBranch(BuildContext &BuildContext,cUIRange ItemRange,sfInt16 Level,bool IsLastNode)
{
	Float32 NodePos[2];
	NodePos[!BuildContext.GridXRow]=(ItemRange.Begin+ItemRange.End)/2;
	Float32 CurLevelIndent=Level*BuildContext.LevelIndent;
	NodePos[BuildContext.GridXRow]=CurLevelIndent+BuildContext.LevelIndent;
	
	if(Level<0){
		fBuildingLastNodeEndPos=ItemRange.End;
		return;
	}
	uIntn CurLevelIndex=Level+1;

	auto BuildingLevelCount=fBuildingBranchs.GetCount();

	if(CurLevelIndex>BuildingLevelCount){
		uIntn NewLevelIndex=BuildingLevelCount;
		fBuildingBranchs.SetCount(CurLevelIndex);

		do{
			auto &Branch=fBuildingBranchs[NewLevelIndex];
			Branch.LevelFinished=true;
			NewLevelIndex++;
			if(BuildContext.GridXRow){
				Branch.Rect.Top=CurLevelIndent;
				Branch.Rect.Bottom=CurLevelIndent+BuildContext.LevelIndent;
				Branch.Rect.Left=fBuildingLastNodeEndPos;
				Branch.Rect.Bottom=fBuildingLastNodeEndPos;
			}
			else{
				Branch.Rect.Left=CurLevelIndent;
				Branch.Rect.Right=CurLevelIndent+BuildContext.LevelIndent;
				Branch.Rect.Top=fBuildingLastNodeEndPos;
				Branch.Rect.Bottom=fBuildingLastNodeEndPos;
			}
		}while(NewLevelIndex<CurLevelIndex);
	}
	else if(CurLevelIndex<BuildingLevelCount){
		for(uIntn i=CurLevelIndex;i<BuildingLevelCount;i++){
			fTreeBranchs.AppendMake(cnVar::MoveCast(fBuildingBranchs[i]));
		}
		fBuildingBranchs.SetCount(CurLevelIndex);
	}
	
	fBuildingLastNodeEndPos=ItemRange.End;
	cnLib_ASSERT(Level<static_cast<sfInt16>(fBuildingBranchs.GetCount()));
	auto &ParentBranch=fBuildingBranchs[Level];
	Float32 &ParentBranchRangeEnd=BuildContext.GridXRow?ParentBranch.Rect.Right:ParentBranch.Rect.Bottom;
	ParentBranchRangeEnd=NodePos[!BuildContext.GridXRow];
	cUIPoint NodePoint={NodePos[0],NodePos[1]};
	ParentBranch.NodeList.AppendMake(NodePoint);
	ParentBranch.LevelFinished=IsLastNode;

}
//---------------------------------------------------------------------------
void cControlTreeBranchData::BuildFinish(BuildContext &BuildContext)
{
	Float32 cUIRectangle::*pBranchRangeEnd=BuildContext.GridXRow?&cUIRectangle::Right:&cUIRectangle::Bottom;
	for(auto &Branch : fBuildingBranchs){
		if(Branch.LevelFinished==false){
			Branch.Rect.*pBranchRangeEnd=fBuildingContentRange.End;
		}
	
		fTreeBranchs.AppendMake(cnVar::MoveCast(Branch));
	}
	for(auto &Branch : fTreeBranchs){
		Branch.Rect.Left+=BuildContext.TreePos.x;
		Branch.Rect.Right+=BuildContext.TreePos.x;
		Branch.Rect.Top+=BuildContext.TreePos.y;
		Branch.Rect.Bottom+=BuildContext.TreePos.y;

		for(auto &Node : Branch.NodeList){
			Node.x+=BuildContext.TreePos.x;
			Node.y+=BuildContext.TreePos.y;
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bcViewTreeList::bcViewTreeList()
	: fInLayout(false)
	, fNeedUpdateTreeViews(false)
	, fNeedUpdateTreeLayout(false)
	, fViewportDisplayIndex(0)
	, fViewportDisplayEndIndex(0)
{
}
//---------------------------------------------------------------------------
bcViewTreeList::~bcViewTreeList()
{
	SetView(nullptr);
}
//---------------------------------------------------------------------------
Float32 bcViewTreeList::GetListOffset(void)const
{
	return fScrollOffset[!fTreeXBranch];
}
//---------------------------------------------------------------------------
void bcViewTreeList::SetListOffset(Float32 ListOffset)
{
	Float32 Offset[2];
	Offset[!fTreeXBranch]=ListOffset;
	SetScrollOffset({Offset[0],Offset[1]});
}
//---------------------------------------------------------------------------
aCls<bcViewTreeList::cNode>* bcViewTreeList::FindNode(Float32 Position)
{
	aCls<cNode>* Node;
	FindNode(Position,Node);
	return Node;
}
//---------------------------------------------------------------------------
bool bcViewTreeList::FindNode(Float32 Position,aCls<cNode>* &Node)
{
	ufInt32 DisplayIndex;
	bool Ret=ViewTreeListDisplayFindAt(Position,DisplayIndex);
	Node=ViewTreeListDisplayIndexToNode(DisplayIndex);
	return Ret;
}
//---------------------------------------------------------------------------
void bcViewTreeList::SetNeedUpdateTreeViews(void)
{
	if(fNeedUpdateTreeViews)
		return;

	fNeedUpdateTreeViews=true;
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
void bcViewTreeList::SetNeedUpdateTreeLayout(void)
{
	if(fNeedUpdateTreeLayout)
		return;

	fNeedUpdateTreeLayout=true;
	SetNeedUpdateContentLayout();
}
//---------------------------------------------------------------------------
void bcViewTreeList::ViewSetup(void)
{
	cScrollViewContainer::ViewSetup();

	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetView(fView);
	}
	else{
		ControlTreeBranchContentSetDefault();
	}
}
//---------------------------------------------------------------------------
void bcViewTreeList::ViewClear(void)
{
	if(fTreeBranchContent!=nullptr){
		fTreeBranchContent->SetView(nullptr);
	}
	
	cScrollViewContainer::ViewClear();
}
//---------------------------------------------------------------------------
void bcViewTreeList::ScrollContentUpdateContentLayout(void)
{
	fInLayout=true;
	cScrollViewContainer::ScrollContentUpdateContentLayout();
	cnLib_ASSERT(fView!=nullptr);

	// Item visible range
	cUIRange ViewportRange;
	ViewportRange.Begin=fScrollOffset[!fTreeXBranch];
	ViewportRange.End=fScrollOffset[!fTreeXBranch]+fScrollViewportSize[!fTreeXBranch];

	ufInt32 RecycleDisplayIndex=fViewportDisplayIndex;
	ufInt32 RecycleDisplayEndIndex=fViewportDisplayEndIndex;

	ViewTreeListDisplayFindAt(ViewportRange.Begin,fViewportDisplayIndex);
	if(ViewTreeListDisplayFindAt(ViewportRange.End,fViewportDisplayEndIndex)){
		fViewportDisplayEndIndex++;
	}
	
	{
		ufInt16 ItemCount=fViewportDisplayEndIndex-fViewportDisplayIndex;
		cUIRange ListRange;
		ListRange.Begin=fScrollLowwerLimit[fTreeXBranch]-fScrollOffset[fTreeXBranch];
		ListRange.End=fScrollUpperLimit[fTreeXBranch]-fScrollOffset[fTreeXBranch];

		cUIRange VisibleRange;
		VisibleRange.Begin=ViewportRange.Begin-fScrollOffset[!fTreeXBranch];
		VisibleRange.End=ViewportRange.End-fScrollOffset[!fTreeXBranch];
		if(fTreeXBranch){
			GridLineSetupStart(ItemCount,1,VisibleRange,ListRange);
		}
		else{
			GridLineSetupStart(1,ItemCount,ListRange,VisibleRange);
		}

		for(ufInt16 i=0;i<ItemCount;i++){
			auto ItemRange=ViewTreeListDisplayQueryRange(i+fViewportDisplayIndex);
			GridLineSetupRange(!fTreeXBranch,i,ItemRange);
		}
		GridLineSetupRange(fTreeYNode,0,ListRange);
		GridLineSetupFinish();
	}

	if(fNeedUpdateTreeLayout){
		fNeedUpdateTreeLayout=false;

		TreePositNode(fRootNode);
	}


	if(fNeedUpdateTreeViews){
		fNeedUpdateTreeViews=false;
		// update all
		ViewTreeListDisplaySetupItems(fViewportDisplayIndex,fViewportDisplayEndIndex);
	}
	else{
		if(fViewportDisplayIndex<RecycleDisplayIndex){
			if(fViewportDisplayEndIndex>RecycleDisplayEndIndex){
				//     |    |		PrevVisible
				// |---|////|---|	NewVisible
				//  Sec1	Sec2

				// Sec1
				ViewTreeListDisplaySetupItems(fViewportDisplayIndex,RecycleDisplayIndex);

				// Sec2
				ViewTreeListDisplaySetupItems(RecycleDisplayEndIndex,fViewportDisplayEndIndex);

				// no change to recycle section
			}
			else if(fViewportDisplayEndIndex>RecycleDisplayIndex){
				//     |    |	PrevVisible
				// |---|///|	NewVisible

				ViewTreeListDisplaySetupItems(fViewportDisplayIndex,RecycleDisplayIndex);
			}
			else{
				//		|   |	PrevVisible
				// |---|		NewVisible

				ViewTreeListDisplaySetupItems(fViewportDisplayIndex,fViewportDisplayEndIndex);
			}
		}
		else if(fViewportDisplayIndex<RecycleDisplayEndIndex){
			if(fViewportDisplayEndIndex>RecycleDisplayEndIndex){
				// |    |		PrevVisible
				//  |///|---|	NewVisible
				
				ViewTreeListDisplaySetupItems(RecycleDisplayEndIndex,fViewportDisplayEndIndex);
			}
			else{
				// |    |		PrevVisible
				//  |//|		NewVisible
			}
		}
		else{
			// |   |		PrevVisible
			//       |---|	NewVisible
				
			ViewTreeListDisplaySetupItems(fViewportDisplayIndex,fViewportDisplayEndIndex);
		}

	}

	auto PrevInUseItemSet=cnVar::MoveCast(fInUseNodeSet);

	TreeKeepVisibleSubNode(fRootNode);

	fInLayout=false;
}
//---------------------------------------------------------------------------
	/*
void bcViewTreeList::ReloadTreeBranchData(void)
{
	auto NodeCount=VisibleRowGetCount();
	if(NodeCount==0){
		fTreeBranchData.Clear();
		return;
	}
	auto FirstNode=TreeNodeGetInfo(0);
	if(FirstNode==nullptr){
		fTreeBranchData.Clear();
		return;
	}

	cUIRange ContentRange;
	ContentRange.Begin=0;
	ContentRange.End=fScrollViewportSize[!fTreeHorizontal];
	fTreeBranchData.BuildStart(ContentRange);

	cControlTreeBranchData::BuildContext TreeBranchBuildContext;
	
	TreeBranchBuildContext.TreePos.x=ContentToView(0,false);
	TreeBranchBuildContext.TreePos.y=ContentToView(0,true);
	TreeBranchBuildContext.GridXRow=fTreeHorizontal;
	TreeBranchBuildContext.LevelIndent=fTreeLevelIndent;

	//Float32 LastNodePrimaryEnd=ContentRange.Begin;
	{
		// build head node list
		aCls<cTreeNodeInfo> *TopNode=FirstNode->Parent;
		cnRTL::cSeqList< aClsRef<cTreeNodeInfo> > Nodes;
		while(TopNode!=nullptr){

			Nodes.AppendMake(TopNode);
		
			TopNode=TopNode->Parent;
		}

		cUIRange TopRange={ContentRange.Begin,ContentRange.Begin};

		for(uIntn i=Nodes.GetCount();i>0;){
			i--;
			fTreeBranchData.BuildBranch(TreeBranchBuildContext,TopRange,Nodes[i]->Level,Nodes[i]->IsLastNode);
		}

	}
	fTreeBranchData.BuildBranch(TreeBranchBuildContext,VisibleRowGetRange(0),FirstNode->Level,FirstNode->IsLastNode);
	
	for(uIntn i=1;i<NodeCount;i++){
		auto NodeInfo=TreeNodeGetInfo(i);
		if(NodeInfo!=nullptr){
			fTreeBranchData.BuildBranch(TreeBranchBuildContext,VisibleRowGetRange(i),NodeInfo->Level,NodeInfo->IsLastNode);
		}
	}
	fTreeBranchData.BuildFinish(TreeBranchBuildContext);
}
	*/
//---------------------------------------------------------------------------
aCls<bcViewTreeList::cNode>* bcViewTreeList::QueryRootNode(void)
{
	return fRootNode;
}
//---------------------------------------------------------------------------
void bcViewTreeList::PositNode(cNode *Node)
{
	cUIRectangle NodeRect;

	ufInt32 DisplayIndex=ViewTreeListNodeToDisplayIndex(Node);
	if(DisplayIndex==IndexNotFound)
		return;

	auto NodeRange=ViewTreeListDisplayQueryRange(DisplayIndex);
	
	if(fTreeXBranch){
		NodeRect.Left=NodeRange.Begin;
		NodeRect.Right=NodeRange.End;
	
		NodeRect.Top=fScrollLowwerLimit[1];
		NodeRect.Bottom=fScrollUpperLimit[1];
	}
	else{
		NodeRect.Top=NodeRange.Begin;
		NodeRect.Bottom=NodeRange.End;
	
		NodeRect.Left=fScrollLowwerLimit[0];
		NodeRect.Right=fScrollUpperLimit[0];
	}
	
	ViewItemSetRectangle(Node,NodeRect);
}
//---------------------------------------------------------------------------
void bcViewTreeList::TreePositNode(cNode *Node)
{
	PositNode(Node);

	for(auto SubNodePair : Node->fNodeMap){
		TreePositNode(SubNodePair.Value);
	}
}
//---------------------------------------------------------------------------
void bcViewTreeList::TreeKeepVisibleSubNode(cNode *Node)
{
	for(auto SubNodePair : Node->fNodeMap){
		auto Node=SubNodePair.Value;
		if(Node->IsVisible()){
			fInUseNodeSet.Insert(Node);
		}
		TreePositNode(Node);
	}
}
//---------------------------------------------------------------------------
bcViewTreeList::cNode::cNode()
	: fOwner(nullptr)
	, fParent(nullptr)
{
}
//---------------------------------------------------------------------------
bcViewTreeList::cNode::~cNode()
{
	for(auto NodePair : fNodeMap){
		NodePair.Value->ClearOwner();
	}

	if(fParent!=nullptr){
		fParent->NodeDeleted(this);
	}
}
//---------------------------------------------------------------------------
void bcViewTreeList::cNode::SetupOwner(bcViewTreeList *Owner,cNode *Parent,ufInt32 NodeIndex)
{
	fOwner=Owner;
	fParent=Parent;
	fOwner->ViewItemAttach(this);
}
//---------------------------------------------------------------------------
void bcViewTreeList::cNode::ClearOwner(void)
{
	fOwner->ViewItemDetach(this);
	fParent=nullptr;
	fOwner=nullptr;
}
//---------------------------------------------------------------------------
void bcViewTreeList::cNode::NodeDeleted(cNode *Node)
{
	auto Pair=fNodeMap.GetPair(Node->fNodeIndex);
	if(Pair!=nullptr){
		if(Pair->Value==Node)
			fNodeMap.RemovePair(Pair);
	}
}
//---------------------------------------------------------------------------
iViewReference* bcViewTreeList::cNode::GetViewReference(void)const
{
	if(fOwner==nullptr)
		return nullptr;
	return fOwner->ViewItemGetViewReference(this);
}
//---------------------------------------------------------------------------
void bcViewTreeList::cNode::SetViewReference(iViewReference *ViewRef)
{
	if(fOwner==nullptr)
		return;
	fOwner->ViewItemSetViewReference(this,ViewRef);
}
//---------------------------------------------------------------------------
aCls<bcViewTreeList::cNode>* bcViewTreeList::cNode::GetParent(void)const
{
	return static_cast<aCls<bcViewTreeList::cNode>*>(fParent);
}
//---------------------------------------------------------------------------
aClsRef<bcViewTreeList::cNode> bcViewTreeList::cNode::QueryNode(uIntn NodeIndex)
{
	if(fOwner==nullptr)
		return nullptr;

	aClsRef<cNode> RetNode;
	auto &CachedNode=fNodeMap[static_cast<ufInt32>(NodeIndex)];
	if(CachedNode!=nullptr){
		RetNode=CachedNode;
	}
	else{
		RetNode=aClsCreate<cNode>();
		RetNode->SetupOwner(fOwner,this,NodeIndex);
		fOwner->PositNode(RetNode);
		CachedNode=RetNode;
	}

	// store to visible row to keep it until next refresh
	fOwner->fInUseNodeSet.Insert(RetNode);

	return RetNode;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void cTreeExpandButton::ButtonBackgroundSetDefault(void)
{
	if(gCreateDefaultTreeExpandButtonVisual!=nullptr){
		SetBackground(gCreateDefaultTreeExpandButtonVisual(this));
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cControlTreeList::cControlTreeList()
	: fControlSource(nullptr)
{
}
//---------------------------------------------------------------------------
cControlTreeList::~cControlTreeList()
{
}
//---------------------------------------------------------------------------
iControlTreeListSource* cControlTreeList::GetControlSource(void)const
{
	return fControlSource;
}
//---------------------------------------------------------------------------
void cControlTreeList::SetControlSource(iControlTreeListSource *Source)
{
	if(fControlSource!=nullptr){
		fControlSource->ControlListNotifySet.Remove(fTreeListNotifyToken);
	}
	fControlSource=Source;
	if(fControlSource!=nullptr){
		fTreeListNotifyToken=fControlSource->ControlListNotifySet.Insert([this]{
			ApplyControlSource();
			SetNeedUpdateTreeViews();
		});
		ApplyControlSource();
	}
	SetNeedUpdateTreeViews();
}
//---------------------------------------------------------------------------
void cControlTreeList::ScrollContentUpdateContentLayout(void)
{
	bcViewTreeList::ScrollContentUpdateContentLayout();
}
//---------------------------------------------------------------------------
cUIRange cControlTreeList::ViewTreeListDisplayQueryRange(ufInt32 DisplayIndex)
{
	cUIRange Range;
	Range.Begin=DisplayIndex*(ItemSize+ItemBorder)+ItemBorder;
	Range.End=Range.Begin+ItemSize;
	return Range;
}
//---------------------------------------------------------------------------
bool cControlTreeList::ViewTreeListDisplayFindAt(Float32 Position,ufInt32 &DisplayIndex)
{
	if(Position<=1){
		DisplayIndex=0;
		return false;
	}

	ufInt32 Pos=static_cast<ufInt32>(Position-ItemBorder);
	DisplayIndex=static_cast<uIntn>(Pos/(ItemSize+ItemBorder));
	return Pos%static_cast<ufInt32>(ItemSize+ItemBorder)!=0;
}
//---------------------------------------------------------------------------
void cControlTreeList::ViewTreeListDisplaySetupItems(uIntn DisplayIndex,ufInt32 DisplayEndIndex)
{
	if(fTreeItemCount<DisplayEndIndex){
		fTreeItemCount=DisplayEndIndex;
		// update row range
		fScrollUpperLimit[!fTreeXBranch]=ItemSize*fTreeItemCount+ItemBorder*(fTreeItemCount+1);
	}

	if(fControlSource==nullptr)
		return;

	//fControlSource->ListSetupItems(ItemIndex,ItemEndIndex-ItemIndex);

	for(ufInt32 i=DisplayIndex;i<DisplayEndIndex;i++){
		//auto Item=QueryNode(i);
			
		//auto ViewReference=fControlSource->ListItemMake(this,i);
		//Item->SetViewReference(ViewReference);
		
	}

	//fControlSource->ListClearItems();
}
//---------------------------------------------------------------------------
aClsRef<cControlTreeList::cNode> cControlTreeList::ViewTreeListDisplayIndexToNode(ufInt32 DisplayIndex)
{
	return nullptr;
}
//---------------------------------------------------------------------------
ufInt32 cControlTreeList::ViewTreeListNodeToDisplayIndex(cNode *Node)
{
	return IndexNotFound;
}
//---------------------------------------------------------------------------
void cControlTreeList::ApplyControlSource(void)
{
//	if(RowSize==0){
//		fScrollLowwerLimit[!fGridXRow]=0;
//		fScrollUpperLimit[!fGridXRow]=0;
//		fScrollNoUpperLimit[!fGridXRow]=false;
//	}
//	else{
//		bool RowCountCorrect=fControlSource->GridGetRowCount(fGridRowCount);
//
//		fScrollLowwerLimit[!fGridXRow]=0;
//		fScrollUpperLimit[!fGridXRow]=RowSize*fGridRowCount+RowBorder*(fGridRowCount+1);
//		fScrollNoUpperLimit[!fGridXRow]=!RowCountCorrect;
//	}
}
//---------------------------------------------------------------------------


#if 0
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cArrayAccumulationCache::cArrayAccumulationCache()
{
	fRoot.LowwerSummation=0;
	fRoot.LowwerSummationCompleted=false;
	fRootBits=0;
}
//---------------------------------------------------------------------------
cArrayAccumulationCache::~cArrayAccumulationCache()
{
}
//---------------------------------------------------------------------------
void cArrayAccumulationCache::Clear(void)
{
	fRoot.Lowwer=nullptr;
	fRoot.LowwerSummation=0;
	fRoot.LowwerSummationCompleted=false;
	fRootBits=0;
}
//---------------------------------------------------------------------------
void cArrayAccumulationCache::debugUpdateValue(void)
{
	fRootBits=2;
	fRoot.LowwerSummation=40;
	fRoot.LowwerSummationCompleted=true;
	{
		auto Item0_3=cnRTL::AutoPtrCreate<cRangeItem>();
		//Item0_3->LowwerSummation=20;
		//Item0_3->LowwerSummationCompleted=true;
		Item0_3->LowwerSummation=10;
		Item0_3->LowwerSummationCompleted=false;

		auto Item0_1=cnRTL::AutoPtrCreate<cRangeItem>();
		//Item0_1->LowwerSummation=10;
		//Item0_1->LowwerSummationCompleted=true;
		Item0_1->LowwerSummation=10;
		Item0_1->LowwerSummationCompleted=true;
		//{
		//	auto Item0=cnRTL::AutoPtrCreate<cRangeItem>();
		//	Item0->LowwerSummation=0;
		//	Item0_1->Lowwer=cnVar::MoveCast(Item0);
		//	auto Item1=cnRTL::AutoPtrCreate<cRangeItem>();
		//	Item1->Mid=0;
		//	Item0_1->Upper=cnVar::MoveCast(Item1);
		//}
		Item0_3->Lowwer=cnVar::MoveCast(Item0_1);
		auto Item2_3=cnRTL::AutoPtrCreate<cRangeItem>();
		//Item2_3->LowwerSummation=10;
		//Item2_3->LowwerSummationCompleted=true;
		Item2_3->LowwerSummation=20;
		Item2_3->LowwerSummationCompleted=false;
		//{
		//	auto Item2=cnRTL::AutoPtrCreate<cRangeItem>();
		//	Item2->Mid=0;
		//	Item2_3->Lowwer=cnVar::MoveCast(Item2);
		//	auto Item3=cnRTL::AutoPtrCreate<cRangeItem>();
		//	Item3->Mid=0;
		//	Item2_3->Upper=cnVar::MoveCast(Item3);
		//}
		Item0_3->Upper=cnVar::MoveCast(Item2_3);

		fRoot.Lowwer=cnVar::MoveCast(Item0_3);
	}
}
//---------------------------------------------------------------------------
void cArrayAccumulationCache::UpdateArrayValue(uIntn Index,uIntn Count,const TValue *Values)
{
}
//---------------------------------------------------------------------------
void cArrayAccumulationCache::SetValue(uIntn Index,TValue Value)
{
	uIntn RangeIndexBit=ScaleRoot(Index);

	cRangeItem *CurRange=&fRoot;
	//uIntn RangeIndex=0;
	bool Upper=false;
	RangeIndexBit>>=1;

	//bool CurRangeLowwerCompleted=fRoot.LowwerSummationCompleted;
	while(RangeIndexBit>0){
		cRangeItem *ParentRange=CurRange;
		//TValue RangeMiddle=RangeLowwer+CurRange->LowwerSummation;
		//CurRangeLowwerCompleted=CurRange->LowwerSummationCompleted;
		if(Upper){
			// upper
			CurRange=ParentRange->Upper;
			//RangeIndex|=RangeIndexBit;
		}
		else{
			// lowwer
			CurRange=ParentRange->Lowwer;
		}
		if(CurRange==nullptr){
			do{
				auto NewItem=cnRTL::AutoPtrCreate<cRangeItem>();
				NewItem->Parent=ParentRange;
				NewItem->LowwerSummation=0;
				NewItem->LowwerSummationCompleted=false;

				CurRange=NewItem;
				if(Upper){
					ParentRange->Upper=cnVar::MoveCast(NewItem);
				}
				else{
					ParentRange->Lowwer=cnVar::MoveCast(NewItem);
				}
				RangeIndexBit>>=1;
				bool Upper=0!=(Index&RangeIndexBit);
				ParentRange=CurRange;
			}while(RangeIndexBit>0);
			break;
		}
		RangeIndexBit>>=1;
		bool Upper=0!=(Index&RangeIndexBit);
	}
	TValue CurValue;
	if(Upper){
		CurValue=CurRange->Parent->LowwerSummation-CurRange->LowwerSummation;
	}
	else{
		CurValue=CurRange->LowwerSummation;
	}
	TValue d=Value-CurValue;
}
//---------------------------------------------------------------------------
void cArrayAccumulationCache::UpdateSummation(uIntn Index,TValue Summation)
{

}
//---------------------------------------------------------------------------
uIntn cArrayAccumulationCache::ScaleRoot(uIntn LeastIndex)
{
	uIntn RangeIndexBit=1<<fRootBits;
	while(LeastIndex>=RangeIndexBit){
		// out of range
		auto LowwerItem=cnRTL::AutoPtrCreate<cRangeItem>();
		fRoot.Lowwer->Parent=LowwerItem;
		LowwerItem->Lowwer=cnVar::MoveCast(fRoot.Lowwer);
		LowwerItem->LowwerSummation=fRoot.LowwerSummation;
		LowwerItem->LowwerSummationCompleted=fRoot.LowwerSummationCompleted;

		LowwerItem->Parent=&fRoot;
		fRoot.Lowwer=cnVar::MoveCast(LowwerItem);
		fRootBits++;
		RangeIndexBit<<=1;
	}
	return RangeIndexBit;
}
//---------------------------------------------------------------------------
//cArrayAccumulationCache::cRangeItem* cArrayAccumulationCache::GetItem(uIntn Index,ufInt8 &Level)
//{
//
//}
//---------------------------------------------------------------------------
cIndexRange cArrayAccumulationCache::GetSummationRangeAt(uIntn Index,TValue &RangeLowwer,TValue &RangeUpper)
{
	uIntn RangeIndexBit=1<<fRootBits;
	if(Index>=RangeIndexBit){
		// out of range
		cIndexRange Range;
		if(fRoot.LowwerSummationCompleted){
			Range.Begin=Range.End=RangeIndexBit;
		}
		else{
			Range.Begin=Range.End=RangeIndexBit-1;
		}
		RangeLowwer=fRoot.LowwerSummation;
		RangeUpper=fRoot.LowwerSummation;
		return Range;
	}

	cRangeItem *CurRange=fRoot.Lowwer;
	if(CurRange==nullptr){
		// no detail
		cIndexRange Range;
		Range.Begin=0;
		if(fRoot.LowwerSummationCompleted){
			Range.End=RangeIndexBit;
		}
		else{
			Range.End=RangeIndexBit-1;
		}
		RangeLowwer=0;
		RangeUpper=fRoot.LowwerSummation;
		return Range;
	}
	RangeLowwer=0;
	RangeUpper=fRoot.LowwerSummation;
	
	uIntn RangeIndex=0;
	RangeIndexBit>>=1;

	cRangeItem *LowwerItem=CurRange;
	uIntn LowwerIndex=0;
	uIntn LowwerIndexBit=RangeIndexBit;
	cRangeItem *UpperItem=CurRange;
	uIntn UpperIndex=0;
	uIntn UpperIndexBit=RangeIndexBit;
	TValue UpperRangeL=0;
	TValue UpperRangeU=fRoot.LowwerSummation;


	bool CurRangeLowwerCompleted=fRoot.LowwerSummationCompleted;
	while(CurRange!=nullptr){

		TValue RangeMiddle=RangeLowwer+CurRange->LowwerSummation;
		CurRangeLowwerCompleted=CurRange->LowwerSummationCompleted;
		
		if(Index&RangeIndexBit){
			// upper
			if(CurRangeLowwerCompleted || LowwerItem->LowwerSummationCompleted){
				LowwerItem=CurRange;
				LowwerIndex=RangeIndex;
				LowwerIndexBit=RangeIndexBit;
			}
			else{
				// keep lowwer range across gap
			}
			
			if(UpperItem->LowwerSummationCompleted){
				UpperItem=CurRange;
				UpperIndex=RangeIndex;
				UpperIndexBit=RangeIndexBit;
				UpperRangeL=RangeLowwer;
				UpperRangeU=RangeUpper;
			}

			RangeLowwer=RangeMiddle;
			CurRange=CurRange->Upper;
			RangeIndex|=RangeIndexBit;
		}
		else{
			// lowwer

			if(CurRangeLowwerCompleted || LowwerItem->LowwerSummationCompleted){
				LowwerItem=CurRange;
				LowwerIndex=RangeIndex;
				LowwerIndexBit=RangeIndexBit;
			}
			if(CurRangeLowwerCompleted==false){
				UpperItem=CurRange;
				UpperIndex=RangeIndex;
				UpperIndexBit=RangeIndexBit;
				UpperRangeL=RangeLowwer;
				UpperRangeU=RangeUpper;
			}

			RangeUpper=RangeMiddle;
			CurRange=CurRange->Lowwer;
		}
		if(RangeIndexBit==1){
			break;
		}
		RangeIndexBit>>=1;
	}
	cIndexRange Range;
	Range.Begin=RangeIndex;
	Range.End=RangeIndex+RangeIndexBit-1;

	// look for upper most item
	if(UpperItem->Upper!=nullptr){
		
		UpperRangeL=UpperRangeL+UpperItem->LowwerSummation;
		UpperIndex|=UpperIndexBit;
		cRangeItem *LookupRange=UpperItem->Upper;
		while(LookupRange!=nullptr){
			UpperIndexBit>>=1;
			if(LookupRange->Lowwer!=nullptr){
				// lowwer
				UpperRangeU=UpperRangeL+LookupRange->LowwerSummation;
				LookupRange=LookupRange->Lowwer;
			}
			else if(LookupRange->Upper!=nullptr){
				// upper
				UpperRangeL=UpperRangeL+LookupRange->LowwerSummation;
				LookupRange=LookupRange->Upper;
				UpperIndex|=UpperIndexBit;
			}
			else{
				// no more branches
				if(LookupRange->LowwerSummationCompleted){
					UpperIndex|=UpperIndexBit;
				}
				else{
					UpperRangeU=UpperRangeL+LookupRange->LowwerSummation;
				}
				break;
			}
		}
		Range.End=UpperIndex+UpperIndexBit-1;
		RangeUpper=UpperRangeU;
	}


	if(CurRangeLowwerCompleted==false){
		// look for gapped lowwer bound
		cRangeItem *LookupRange=LowwerItem->Lowwer;

		while(LookupRange!=nullptr){
			LowwerIndexBit>>=1;
			if(LookupRange->Upper!=nullptr){
				// upper
				LookupRange=LookupRange->Upper;
				LowwerIndex|=LowwerIndexBit;
			}
			else if(LookupRange->Lowwer!=nullptr){
				// lowwer
				LookupRange=LookupRange->Lowwer;
			}
			else{
				// no more branches
				if(LookupRange->LowwerSummationCompleted){
					LowwerIndex|=LowwerIndexBit;
				}
				break;
			}
		}
		

		Range.Begin=LowwerIndex;
	}
	return Range;
}
//---------------------------------------------------------------------------
cIndexRange cArrayAccumulationCache::FindSummationIndexRange(TValue &RangeLowwer,TValue &RangeUpper,TValue Summation)
{
	uIntn RangeIndex=0;
	uIntn RangeIndexBit=1<<fRootBits;

	RangeLowwer=0;
	RangeUpper=fRoot.LowwerSummation;
	
	bool CurRangeLowwerCompleted=fRoot.LowwerSummationCompleted;
	
	cRangeItem *LastLowwerNode=&fRoot;
	uIntn LastLowwerIndex=RangeIndex;
	uIntn LastLowwerIndexBit=RangeIndexBit;

	if(Summation<fRoot.LowwerSummation){
		uIntn LastLowwerIndex=RangeIndex;
		uIntn LastLowwerIndexBit=RangeIndexBit;

		cRangeItem *LookupRange=fRoot.Lowwer;

		while(LookupRange!=nullptr){
			if(RangeIndexBit==1){
				break;
			}
			RangeIndexBit>>=1;

			CurRangeLowwerCompleted=LookupRange->LowwerSummationCompleted;
			TValue RangeMiddle=RangeLowwer+LookupRange->LowwerSummation;
			if(Summation<=RangeMiddle){
				RangeUpper=RangeMiddle;
				LookupRange=LookupRange->Lowwer;
			}
			else{	//  Summation>RangeMiddle
				RangeLowwer=RangeMiddle;
				if(CurRangeLowwerCompleted || LastLowwerNode->LowwerSummationCompleted){
					LastLowwerNode=LookupRange;
					LastLowwerIndex=RangeIndex;
					LastLowwerIndexBit=RangeIndexBit;
				}
				else{
					// keep lowwer range across gap
				}
				LookupRange=LookupRange->Upper;
				RangeIndex|=RangeIndexBit;
			}
		}
	}
	// in range
	cIndexRange Range;
	Range.Begin=RangeIndex;
	Range.End=RangeIndex+RangeIndexBit-1;

	if(CurRangeLowwerCompleted==false){
		// look for gapped lowwer bound
		cRangeItem *LookupRange=LastLowwerNode->Lowwer;
		LastLowwerIndexBit>>=1;

		while(LookupRange!=nullptr){
			LastLowwerIndexBit>>=1;
			if(LookupRange->Upper!=nullptr){
				// upper
				LookupRange=LookupRange->Upper;
				LastLowwerIndex|=LastLowwerIndexBit;
			}
			else if(LookupRange->Lowwer!=nullptr){
				// lowwer
				LookupRange=LookupRange->Lowwer;
			}
			else{
				// no more branches
				if(LookupRange->LowwerSummationCompleted){
					LastLowwerIndex|=LastLowwerIndexBit;
				}
				break;
			}
		}
		

		Range.Begin=LastLowwerIndex;
	}
	return Range;
}
#endif // 0
//---------------------------------------------------------------------------
