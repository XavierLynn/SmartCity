// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonUEFunLib.generated.h"

/**
 * 
 */
UCLASS()
class JSONUE_API UJsonUEFunLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "JsonUE" , meta = (ToolTip = "parse json string,if jsonStr is empty then JsonObject is empty"))
	static bool Parse(UJsonPath*& JsonObject,FString jsonStr="");

	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "parse json file,if jsonFile is empty then JsonObject is empty"))
	static bool ParseFile(UJsonPath*& JsonObject, FString jsonFile = "");



};
