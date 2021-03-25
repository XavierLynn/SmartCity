// Copyright 2020 byTangs, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RTIBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class TEXTURERUNTIMEIMPORTER_API URTIBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable,Category="RuntimeImportTexture")
	static UTexture2D* ImportTexture(const FString& Filename);
};
