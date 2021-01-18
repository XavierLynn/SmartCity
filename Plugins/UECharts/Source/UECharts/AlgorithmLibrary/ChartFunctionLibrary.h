// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Blueprint/UserWidget.h"

#include "ChartFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UECHARTS_API UChartFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Chart|CeilNumber")
	static float CeilNumber(float Number);

	UFUNCTION(BlueprintPure, Category = "Chart|CeilNumber")
		static FString NumberToString(float Number,int DecimalDigits = 2);

	UFUNCTION(BlueprintCallable, meta = (AdvancedDisplay = "4"), Category = "Painting")
		static void ChartDrawLine(UPARAM(ref) FPaintContext& Context, FVector2D PositionA, FVector2D PositionB, FLinearColor Tint = FLinearColor::White, bool bAntiAlias = true, float Thickness = 1.0f,int SubLayer = 0);
	//UFUNCTION(BlueprintCallable, Category = "Chart|GetImageSize")
	//	static void GetImageSize(UPARAM(ref) UTexture2D& CurTexture);
	UFUNCTION(BlueprintCallable, meta = (Keyword = "Sort map by value"), Category = "MathBPLibrary|Map")
		static void MathStringSortMap(UPARAM(ref) TMap<FString, FString>& FStringMap, bool InReverseOrder, bool InByValue = true);

	UFUNCTION(BlueprintCallable, Meta = (Keyword = "Regular To Find Value"), Category = "Regular")
		static bool Regular(const FString CurStr, const FString Re);

	UFUNCTION(BlueprintCallable, Meta = (Keywords = "String Limite"), Category = "StringLimiteLen")
		static FString StringLimiteLen(FString CurStr, int Len);

	//UFUNCTION(BlueprintCallable, meta = (Keywords = "SplineToActorDistance"), Category = "SplineToActorDis")
	//	static AActor FindNearSplineActor(AActor CurSplline);

};
