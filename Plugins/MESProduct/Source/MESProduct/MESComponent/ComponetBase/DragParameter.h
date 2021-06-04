// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MESComponetBase.h"
#include "DragParameter.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UDragParameter : public UDragDropOperation
{
	GENERATED_BODY()
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "∂®÷∆ Ù–‘", Meta = (DisplayName = "DragOffset"))
		FVector2D DragOffset;
	
	
};
