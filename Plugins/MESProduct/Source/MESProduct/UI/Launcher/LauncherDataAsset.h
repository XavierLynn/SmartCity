// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LauncherDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MESPRODUCT_API ULauncherDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Version;
};
