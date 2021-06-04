// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MESEchartsToolFuncLib.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESEchartsToolFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure)
	static TArray<float> LiRegulate(float MinValue, float MaxValue, int32 SpliteNum = 5);

	UFUNCTION(BlueprintPure)
		static TArray<float> BLiRegulate(float MinValue, float MaxValue, int32 SpliteNum = 5);

	UFUNCTION(BlueprintPure)
		static bool IsPointInCircle(FVector2D SrcPoint, FVector2D StartDir, FVector2D EndPointVec, float InnerRad, float OutRad, float Angle,float& OutAngle);

};
