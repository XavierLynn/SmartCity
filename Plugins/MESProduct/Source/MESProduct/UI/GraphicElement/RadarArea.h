// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/UI/GraphicElement/MeshUIBase.h"
#include "RadarArea.generated.h"

/**
 * 
 */


UCLASS()
class MESPRODUCT_API URadarArea : public UMeshUIBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "数据"))
		TArray<float> Data;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线宽"))
		float LineWidth = 2.f; //线段宽度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线颜色"))
		FColor LineColor = FColor::White;

protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

	FVector2D GetPoint(float radius,float angle);
	
private:
	float outerRadius;

public :
	TArray<FSlateVertex> Points;
	
};
