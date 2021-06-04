// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/UI/GraphicElement/MeshUIBase.h"
#include "ChartAxis.generated.h"

UENUM(BlueprintType)
enum class EAxisArrow : uint8
{
	Up = 0 UMETA(DisplayName = "向上"),
	Right = 1 UMETA(DisplayName = "向右"),
	Down = 2  UMETA(DisplayName = "向下"),
	Left = 3 UMETA(DisplayName = "向左"),
};

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UChartAxis : public UMeshUIBase
{
	GENERATED_BODY()
	
public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "刻度数"))
		int KeDuNum = 2; //
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "刻度宽"))
		float KeDuKuan = 5.f;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "刻度朝向"))
		EAxisArrow KeDuArrow = EAxisArrow::Right;
	
protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

public:
	TArray<FVector2D> KeDuPoistion;
};
