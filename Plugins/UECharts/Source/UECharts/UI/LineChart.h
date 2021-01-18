// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UECharts/UI/ChartBaseWidget.h"
#include "LineChart.generated.h"


UCLASS()
class UECHARTS_API ULineChart : public UChartBaseWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "数据"))
		TArray<float> Data;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线宽"))
		float LineWidth = 2.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "单独配置线颜色"))
		bool UseLineColor = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线颜色"))
		FColor LineColor = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "是否虚线"))
		bool IsBroken = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "空白长度"))
		float XuXianLen = 5.f; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "虚线长度"))
		float XuXianGap = 10.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "是否曲线"))
		bool IsCurve = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "是否面积图"))
		bool IsArea = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "面积顶部透明度"))
		float AreaTop = 0.5; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "面积底部透明度"))
		float AreaBottom = 0.5; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "是否显示点"))
		bool IsShowPoint = false; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "点半径"))
		float PointRidus = 20; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "是否变大"))
		bool IsBig = false; 
public:

	UFUNCTION(BlueprintCallable, Category = "ChartConfing") 
		TArray<FVector2D> GetPointLocation();


protected:

	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

	TArray<FVector2D> PointLocations;
private:
	TArray<FSlateVertex> PointList(TArray<FSlateVertex> points, int pointSize, float MaxY, float MinY);
	FSlateVertex Interp(TArray<FSlateVertex>& pts, float t);

	void CheckPointXuXianLen(TArray<FSlateVertex>& NewPoint, FSlateVertex& LastPos, FSlateVertex NextPos);

	void DrawPoint(float Ridius, FVector2D Origin);


};
