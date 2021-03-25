// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RTIBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMENGINE_API URTIBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * 加载本地图片,本地绝对路径
	 */
	UFUNCTION(BlueprintCallable,Category="RuntimeImportTexture")
	static UTexture2D* ImportTexture(const FString& Filename);

	/**
	 * 从内容浏览器路径加载图片
	 */
	UFUNCTION(BlueprintCallable,Category="RuntimeImportTexture")
	static UTexture2D* LoadTextureFormContentPath(UObject* Outer,const FString& TexturePath);
};
