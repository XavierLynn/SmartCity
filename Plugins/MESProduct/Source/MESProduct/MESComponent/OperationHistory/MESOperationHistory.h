// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MESProduct/MESComponent/ComponetBase/MESComponetBase.h"
#include "MESOperationHistory.generated.h"

UENUM(BlueprintType)
enum class EOperationType : uint8
{
	None,
	Add,
	Delete,
	Update
};


USTRUCT(BlueprintType)
struct FOperationNode
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
		class UMESComponetBase* ComponentBase;
	UPROPERTY(BlueprintReadWrite)
		FVector2D Position;
	UPROPERTY(BlueprintReadWrite)
		FVector2D Size;
	UPROPERTY(BlueprintReadWrite)
		EOperationType OperationType;

	void Init(class UMESComponetBase* _ComponentBase, EOperationType _OperationType)
	{
		if (_ComponentBase && _ComponentBase->IsValidLowLevelFast())
		{
			Position = _ComponentBase->CommonData.Pos;
			Size = _ComponentBase->CommonData.Size;
		}
		ComponentBase = _ComponentBase;
		OperationType = _OperationType;
	}

	void Init(class UMESComponetBase* _ComponentBase, FVector2D _Position, FVector2D _Size, EOperationType _OperationType)
	{
		ComponentBase = _ComponentBase;
		Position = _Position;
		Size = _Size;
		OperationType = _OperationType;
	}

	bool operator==(const FOperationNode& OtherNode) const
	{
		if (ComponentBase == nullptr || !ComponentBase->IsValidLowLevelFast())
		{
			return false;
		}

		if (OtherNode.ComponentBase == nullptr || !OtherNode.ComponentBase->IsValidLowLevelFast())
		{
			return true;
		}

		return ComponentBase->CommonData.UUID == OtherNode.ComponentBase->CommonData.UUID;
	}
};


/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESOperationHistory : public UObject
{
	GENERATED_BODY()

		UMESOperationHistory();
	
public:
	UFUNCTION(BlueprintCallable, Category = "MESUtil | OperationHistory", BlueprintPure, meta = (DisplayName = "GetOperationHistory"))
		static UMESOperationHistory* Get();

	// Use this function to store an update operation history node before change.
	UFUNCTION(BlueprintCallable)
		void CreateUpdateOperation(class UMESComponetBase* ComponentBase, FVector2D PreviousPos, FVector2D PreviousSize);

	// Use this function to store an add or delete operation history node.
	UFUNCTION(BlueprintCallable)
		void CreateOperation(class UMESComponetBase* ComponentBase, EOperationType OperationType);

	// Undo an operation and push it to our RedoList.
	UFUNCTION(BlueprintCallable)
		FOperationNode UndoOperation();

	UFUNCTION(BlueprintCallable)
		FOperationNode RedoOperation();

	UFUNCTION(BlueprintCallable)
		void SetOperationMaxNum(int NewMaxNum = 30);

	UFUNCTION(BlueprintCallable)
		void ClearOperation();

private:
	UPROPERTY()
		TArray<FOperationNode> UndoList;
	UPROPERTY()
		TArray<FOperationNode> RedoList;

	int OperationMaxNum = 30;
};
