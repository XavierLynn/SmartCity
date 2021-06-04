// Fill out your copyright notice in the Description page of Project Settings.


#include "MESTreeView.h"
#include "Misc/TextFilter.h"

UMESTreeView::UMESTreeView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UMESTreeView::OnItemClickedInternal(UObject* ListItem)
{
	UListView::OnItemClickedInternal(ListItem);
}

void UMESTreeView::OnWidgetRebuilt()
{
	Super::OnWidgetRebuilt();
	if (MyTreeView)
	{
		SetScrollbarVisibility(ESlateVisibility::Collapsed);
		MyTreeView->SetScrollbarVisibility(EVisibility::Collapsed);
	}
}

void UMESTreeView::ExpandSelectItem(UObject* ListItem)
{
	// If the clicked item has children, expand it now as part of the click
	if (ensure(MyTreeView.IsValid()))
	{
		// The item was clicked, implying that there should certainly be a widget representing this item right now
		TSharedPtr<ITableRow> RowWidget = MyTreeView->WidgetFromItem(ListItem);
		if (ensure(RowWidget.IsValid()) && RowWidget->DoesItemHaveChildren() > 0)
		{
			const bool bNewExpansionState = !MyTreeView->IsItemExpanded(ListItem);
			MyTreeView->SetItemExpansion(ListItem, bNewExpansionState);
		}
	}
}

void UMESTreeView::ExpandAllItems()
{
	if (MyTreeView.IsValid())
	{
		for (UObject* ListItem : GetListItems())
		{
			ExpandSelectItem(ListItem);
		}
	}
}

bool UMESTreeView::DoesItemHaveChildren(UObject * Item)
{
	if (MyTreeView.IsValid())
	{
		TSharedPtr<ITableRow> RowWidget = MyTreeView->WidgetFromItem(Item);
		return RowWidget.IsValid() && RowWidget->DoesItemHaveChildren() > 0;
	}

	return false;
}

void UMESTreeView::SetSingleExpandedItem(UObject * Item)
{
	if (MyTreeView.IsValid())
	{
		MyTreeView->SetSingleExpandedItem(Item);
	}
}
