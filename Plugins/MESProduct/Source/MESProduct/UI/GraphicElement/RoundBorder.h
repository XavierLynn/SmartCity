// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/UI/GraphicElement/MeshUIBase.h"
#include "RoundBorder.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API URoundBorder : public UMeshUIBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "线条宽度"))
	float LineWidth = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ChartConfig", Meta = (DisplayName = "MainUIScale"))
	float MainUIScale = 1.f;

protected:
	virtual void OnPopulateMesh(const FGeometry& AllottedGeometry, TArray<FSlateVertex>& vertex, TArray<SlateIndex>& index) override;
};
