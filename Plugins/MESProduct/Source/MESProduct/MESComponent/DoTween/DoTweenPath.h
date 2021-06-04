// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DotweenBase.h"
#include "DoTweenPath.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FHandleFPath_Single, FVector, curValue, int, key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleFPath, FVector, curValue,UDoTweenBase*, base);
UCLASS(BlueprintType, Blueprintable)
class MESPRODUCT_API UDoTweenPath: public UDoTweenBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		static UDoTweenPath* CallDoPath(float durTime, FVector start, TArray<FVector> Path, bool Loop,
			const FHandleFPath_Single&OutOnComplete, const FHandleFPath_Single& OutOnUpdate);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "false"))
		static UDoTweenPath* DoPath(float durTime, FVector start, TArray<FVector> Path,bool Loop);

	UPROPERTY(BlueprintAssignable)
		FHandleFPath OnComplete;

	UPROPERTY(BlueprintAssignable)
		FHandleFPath OnUpdate;

	FVector StartValue;
	FVector EndValue;
	TArray<FVector> PathValue;
	//存储 分段路径总和
	TArray<float> PathLenths;

	float PathTotalLenth;
	//计数，当前已经移动到哪个Vector了
	int32 Count;
	bool bLoop;
	FVector LastPos;
protected:
	virtual void UpdateFunc(float deltaTime) override;
private:
	
};
