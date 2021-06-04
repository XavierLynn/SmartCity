// Fill out your copyright notice in the Description page of Project Settings.


#include "MESOperationHistory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

UMESOperationHistory::UMESOperationHistory()
{

}

UMESOperationHistory* UMESOperationHistory::Get()
{
	static UMESOperationHistory* Instance = nullptr;
	if (Instance == nullptr)
	{
		Instance = NewObject<UMESOperationHistory>();
		Instance->AddToRoot();
	}
	return Instance;
}

void UMESOperationHistory::CreateUpdateOperation(UMESComponetBase * ComponentBase, FVector2D PreviousPos, FVector2D PreviousSize)
{
	if (ComponentBase == nullptr || !ComponentBase->IsValidLowLevelFast())
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateOperation failed, ComponentBase is null"));
		return;
	}

	if (ComponentBase->CommonData.Pos.Equals(PreviousPos, 0.1f) &&
		ComponentBase->CommonData.Size.Equals(PreviousSize, 0.1f))
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateOperation failed, ComponentBase has not been updated"));
		return;
	}
	for (int i = UndoList.Num() - 1; i >= 0; --i)
	{
		FOperationNode& UndoNode = UndoList[UndoList.Num() - 1];
		if (UndoNode.ComponentBase && UndoNode.ComponentBase->IsValidLowLevelFast())
		{
			if (PreviousPos.Equals(UndoNode.Position) && PreviousSize.Equals(UndoNode.Size) && UndoNode.OperationType == EOperationType::Update)
			{
				UE_LOG(LogTemp, Warning, TEXT("CreateOperation failed, ComponentBase has not been updated"));
				return;
			}
		}
	}

	if (UndoList.Num() == OperationMaxNum)
	{
		UndoList.RemoveAt(0);
	}

	FOperationNode OperationNode;
	OperationNode.Init(ComponentBase, PreviousPos, PreviousSize, EOperationType::Update);

	UndoList.Add(OperationNode);
	RedoList.Reset();
}

void UMESOperationHistory::CreateOperation(class UMESComponetBase* ComponentBase, EOperationType OperationType)
{
	if (ComponentBase == nullptr || !ComponentBase->IsValidLowLevelFast())
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateOperation failed, ComponentBase is null"));
		return;
	}

	if (UndoList.Num() == OperationMaxNum)
	{
		UndoList.RemoveAt(0);
	}

	FOperationNode OperationNode;
	OperationNode.Init(ComponentBase, OperationType);

	UndoList.Add(OperationNode);
	RedoList.Reset();
}

FOperationNode UMESOperationHistory::UndoOperation()
{
	FOperationNode Result;
	bool OperationSuccess = false;
	while (!OperationSuccess && UndoList.Num() > 0)
	{
		FOperationNode& UndoNode = UndoList[UndoList.Num() - 1];
		if (UndoNode.ComponentBase && UndoNode.ComponentBase->IsValidLowLevelFast())
		{
			FVector2D RedoPosition = UndoNode.ComponentBase->CommonData.Pos;
			FVector2D RedoSize = UndoNode.ComponentBase->CommonData.Size;
			EOperationType RedoOperationType = UndoNode.OperationType;

			FOperationNode RedoNode;
			RedoNode.Init(UndoNode.ComponentBase, RedoPosition, RedoSize, RedoOperationType);
			RedoList.Add(RedoNode);

			switch (UndoNode.OperationType)
			{
			case EOperationType::Add:
				// return to Blueprint and do something;
				break;
			case EOperationType::Delete:
				// return to Blueprint and do something;
				break;
			case EOperationType::Update:
				UndoNode.ComponentBase->CommonData.Pos = UndoNode.Position;
				UndoNode.ComponentBase->CommonData.Size = UndoNode.Size;
				break;
			}
			OperationSuccess = true;
			Result = UndoNode;
		}
		UndoList.RemoveAt(UndoList.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("UndoList Num: %d, RedoList Num: %d"), UndoList.Num(), RedoList.Num());
	}

	return Result;
}

FOperationNode UMESOperationHistory::RedoOperation()
{
	FOperationNode Result;
	bool OperationSuccess = false;

	while (!OperationSuccess && RedoList.Num() > 0)
	{
		FOperationNode& RedoNode = RedoList[RedoList.Num() - 1];
		if (RedoNode.ComponentBase && RedoNode.ComponentBase->IsValidLowLevelFast())
		{
			FVector2D UndoPosition = RedoNode.ComponentBase->CommonData.Pos;
			FVector2D UndoSize = RedoNode.ComponentBase->CommonData.Size;
			EOperationType UndoOperationType = RedoNode.OperationType;

			FOperationNode UndoNode;
			UndoNode.Init(RedoNode.ComponentBase, UndoPosition, UndoSize, UndoOperationType);
			UndoList.Add(UndoNode);

			switch (RedoNode.OperationType)
			{
			case EOperationType::Add:
				// return to Blueprint and do something;
				break;
			case EOperationType::Delete:
				// return to Blueprint and do something;
				break;
			case EOperationType::Update:
				RedoNode.ComponentBase->CommonData.Pos = RedoNode.Position;
				RedoNode.ComponentBase->CommonData.Size = RedoNode.Size;
				break;
			}

			OperationSuccess = true;
			Result = RedoNode;
		}
		RedoList.RemoveAt(RedoList.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("UndoList Num: %d, RedoList Num: %d"), UndoList.Num(), RedoList.Num());
	}

	return Result;
}

void UMESOperationHistory::SetOperationMaxNum(int NewMaxNum)
{
	OperationMaxNum = NewMaxNum;
	if (UndoList.Num() > NewMaxNum)
	{
		UndoList.RemoveAt(0, (UndoList.Num() - NewMaxNum));
	}
	if (RedoList.Num() > NewMaxNum)
	{
		RedoList.RemoveAt(0, (RedoList.Num() - NewMaxNum));
	}
}

void UMESOperationHistory::ClearOperation()
{
	UndoList.Reset();
	RedoList.Reset();
}
