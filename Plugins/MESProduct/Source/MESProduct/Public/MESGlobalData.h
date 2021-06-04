// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MESGlobalData.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESGlobalData : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool IsDesignMode;
	
	UFUNCTION(BlueprintCallable, Category = "MESGlobalData")
			static void SetDesignMode(bool IsDesign);

	UFUNCTION(BlueprintCallable, Category = "MESGlobalData")
		static bool GetDesignMode();
};
