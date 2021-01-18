// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UECharts/UI/ChartBaseWidget.h"
#include "ChartsAxis.generated.h"

UENUM(BlueprintType)
enum class EAxisInterArrow : uint8
{
	Up = 0 UMETA(DisplayName = "向上"),
	Right = 1 UMETA(DisplayName = "向右"),
	Down = 2  UMETA(DisplayName = "向下"),
	Left = 3 UMETA(DisplayName = "向左"),
};


UCLASS()
class UECHARTS_API UChartsAxis : public UChartBaseWidget
{
	GENERATED_BODY()
	
public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "刻度数"))
		int KeDuNum = 2; 
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "刻度宽"))
		float KeDuKuan = 5.f;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "刻度朝向"))
		EAxisInterArrow KeDuArrow = EAxisInterArrow::Right;
	
protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

public:
	TArray<FVector2D> KeDuPoistion;
};
