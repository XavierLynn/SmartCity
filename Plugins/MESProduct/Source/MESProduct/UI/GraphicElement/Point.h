// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/UI/GraphicElement/MeshUIBase.h"
#include "Engine/Texture2DDynamic.h"
#include "Point.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UPoint : public UMeshUIBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "描边宽度"))
	float OutLineWidth = 1.f; //描边宽度 向内部开始描边

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "描边颜色"))
	FColor OutLineColor; //描边颜色

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "边缘渐透"))
	float AntiEdget = 1.f; //边缘渐透主要是为了方便 抗锯齿用的
	
protected:
	
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;

private:
	void DrawCircle(float inner,float outer,FColor inColor, FColor ouColor);

};
