// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ConfigManager.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UConfigManager : public UObject
{
	GENERATED_BODY()

		UConfigManager();

public:
	static UConfigManager* Get();

public:
	UPROPERTY(BlueprintReadWrite)
		class ULauncherDataAsset* LauncherConfig;

private:
	UFUNCTION(BlueprintCallable)
		static UConfigManager* GetConfigManager();
};
