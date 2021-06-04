// Fill out your copyright notice in the Description page of Project Settings.


#include "MESStyleBase.h"
#include "InputCore/Classes/InputCoreTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "DragParameter.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

TMap<uint32, TArray<UMESStyleBase*>> UMESStyleBase::DicStyle = TMap<uint32, TArray<UMESStyleBase*>>();

FReply UMESStyleBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UPanelWidget* parent = GetParent();
	if (parent != nullptr)
	{
		int32 UUID = parent->GetUniqueID();
		if (DicStyle.Contains(UUID))
		{
			TArray<UMESStyleBase*> array = DicStyle[UUID];
			for (int i = 0; i < array.Num(); i++)
			{
				if (array[i]->SelectFrame == nullptr)
					continue;
				if (array[i] == this)
				{
					array[i]->SelectFrame->SetVisibility(ESlateVisibility::Visible);
				}
				else
				{
					array[i]->SelectFrame->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	DragOffset = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	OnClickCallBack.Broadcast(this);
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UMESStyleBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragParameter* op = Cast<UDragParameter>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragParameter::StaticClass()));
	UMESStyleBase* DragWidget = CreateWidget<UMESStyleBase>(GetWorld(),this->GetClass());
	DragWidget->IsDraging = true;
	op->DefaultDragVisual = DragWidget;
	op->Payload = this;
	op->Pivot = EDragPivot::CenterCenter;
	op->DragOffset = DragOffset;
	OutOperation = op;
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

void UMESStyleBase::NativeConstruct()
{
	UPanelWidget* parent = GetParent();
	if (parent != nullptr)
	{
		ParentUUID = parent->GetUniqueID();
		if (!DicStyle.Contains(ParentUUID))
		{
			TArray<UMESStyleBase*> array;
			DicStyle.Add(ParentUUID, array);
		}
		DicStyle[ParentUUID].Add(this);
	}
	Super::NativeConstruct();
}

void UMESStyleBase::NativeDestruct()
{
	if (DicStyle.Contains(ParentUUID))
	{
		DicStyle[ParentUUID].Remove(this);
	}	
	Super::NativeDestruct();
}

bool UMESStyleBase::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	UCanvasPanel* TempRootWidget = (UCanvasPanel*)GetRootWidget();
	UTextBlock* TempTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TextBlock_230")));
	if (TempTextBlock)
	{
		TextName = TempTextBlock->GetText().ToString();
		return true;
	}
	return true;
}

