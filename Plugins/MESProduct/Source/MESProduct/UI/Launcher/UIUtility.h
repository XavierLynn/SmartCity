// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UIUtility.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UUIUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	
	UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static bool ScreenShotWithUI(FVector2D  MinPos, FVector2D MaxPos);
	UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static bool HighResScreenshotByRect(FVector2D  MinPos, FVector2D MaxPos);
	UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static bool HighResScreenshotByReslution(float Width,float Height);
};
