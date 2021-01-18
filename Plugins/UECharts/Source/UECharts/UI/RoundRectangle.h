// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UECharts/UI/ChartBaseWidget.h"
#include "RoundRectangle.generated.h"

UCLASS()
class UECHARTS_API URoundRectangle : public UChartBaseWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "圆角半径"))
		float FixRadius = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "左上圆角"))
		bool ShowLeftTop = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "右上圆角"))
		bool ShowRightTop = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "左下圆角"))
		bool ShowLeftBottom = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "右下圆角"))
		bool ShowRightBottom = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "虚线"))
		bool IsBroken = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "实现长度"))
		float LineLen = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "虚线长度"))
		float SpaceLen = 5;
	
protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

private:
	float radius;

	void SetColor(FSlateVertex pos[]);

	void DrawAngle(FVector2D centerPos, float startAngle, float endAngle, float curRadius, float stepAngle);
};

