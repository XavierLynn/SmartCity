// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MediaAssets/Public/MediaTexture.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MESPathUtility.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESPathUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path"/*, meta = (WorldContext = "WorldContextObject")*/)
		static FString GetProductBasePath();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetProjectBasePath();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetStandaloneProjectDBPath(FString ProjectName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetStandaloneProjectBaseDBPath();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetProductDataPath();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetStandaloneProjectDataPath(FString ProjectName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil")
			static FString GetScreenShotPath();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetStandaloneProjectResourcePath(FString ProjectName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetServerProjectDataPath(FString ProjectName);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "MESUtil | Path")
		static FString GetServerProjectDBPath(FString ProjectName);
};
