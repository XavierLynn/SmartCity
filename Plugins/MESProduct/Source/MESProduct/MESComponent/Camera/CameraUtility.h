// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MES_CameraController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CameraUtility.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UCameraUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void SetPos(const UObject* WorldContextObject, FVector Location, FRotator Rotation);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void MoveToPos(const UObject* WorldContextObject, FVector Location, FRotator Rotation, float Seconds, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void MoveToBound(const UObject* WorldContextObject, struct FBoxSphereBounds Bounds, float Seconds, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void PlayAni(const UObject* WorldContextObject, FString AniName, FCameraActionCompleteDynamicDelegate Callback);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void PauseAni(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void ResetAni(const UObject* WorldContextObject, bool IsContinueToPlay = true);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static bool IsPlayingAni(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void UpdateRotateAroundMode(const UObject* WorldContextObject, EMESCameraControllerRotateAroundMode NewMode);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void UpdateRotateAroundPoint(const UObject* WorldContextObject, FVector SpecifyPoint);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static void JumpToAni(const UObject* WorldContextObject, FString AniName, float DurationPercentage, FCameraActionCompleteDynamicDelegate Callback, bool IsContinueToPlay = true);
	UFUNCTION(BlueprintCallable, Category = "CameraUtility", meta = (WorldContext = "WorldContextObject"))
		static float GetAniPercentage(const UObject* WorldContextObject);
};
