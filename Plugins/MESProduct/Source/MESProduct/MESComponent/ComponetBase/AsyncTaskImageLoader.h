// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/AsyncTaskDownloadImage.h"
#include "Async/Future.h"
#include "PixelFormat.h"
#include "Engine/Texture2D.h"
#include "AsyncTaskImageLoader.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UAsyncTaskImageLoader : public UAsyncTaskDownloadImage
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTaskImageLoader* DownloadOrLoadImageFromDisk(FString URL);

	void LoadFromDiskAsync(FString URL);
	bool LoadFromDisk(const FString& ImagePath);

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool LoadFromDiskSuccess = false;
};
