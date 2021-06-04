// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/UI/GraphicElement/MeshUIBase.h"
#include "Slate/SlateVectorArtData.h"
#include "RadarBg.generated.h"
/**
 * 
 */
UCLASS()
class MESPRODUCT_API URadarBg : public UMeshUIBase
{
	GENERATED_BODY()

public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线颜色"))
	FColor LineColor = FColor::Black;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线宽"))
	float LineWidth = 2.f; //线段宽度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "连接线"))
	bool ShowConnect = false; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "边缘个数"))
	int EdgeCount = 3;//边缘个数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "梯度个数"))
	int StepCount = 3;//梯度个数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "梯度色"))
	bool StepColor = true;
	
protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

private:
	void DrawPolygon(float inner, float outer,FColor color,bool LimitColor);
	void DrawConnectLine(float outer, float w);
	
	float outerRadius;
};
