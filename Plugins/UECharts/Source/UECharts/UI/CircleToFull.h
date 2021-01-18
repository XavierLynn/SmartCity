﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UECharts/UI/ChartBaseWidget.h"
#include "Slate/SlateVectorArtData.h"

#include "CircleToFull.generated.h"




UCLASS()
class UECHARTS_API UCircleToFull : public UChartBaseWidget
{
	GENERATED_BODY()

public:	

	UCircleToFull();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "环形宽度"))
	float CircleWidth = 10.f;//环形宽度
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "起始角"))
	float StartAngle = 0.f;//
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "起始->圆角"))
	bool CurveStart;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "结束角"))
	float EndAngle = 180.f;//

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "结束->角"))
	bool CurveEnd;//


protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

private:
	void DrawFullCircle( float outer, float inner, float start, float end, float offsetX, float offsetY,bool OuterAnti,bool innerAnti);
};
