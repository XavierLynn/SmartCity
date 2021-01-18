// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AsyncToolBase.h"
#include "AsyncFloat.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_TwoParams(FHandleFloat_Single, float, curValue, int, key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleFloat, float, curValue, UAsyncToolBase*, base);


UCLASS()
class UECHARTS_API UAsyncFloat : public UAsyncToolBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncFloat* DoFloat(float durTime, float start, float end);

	UFUNCTION(BlueprintCallable)
		static UAsyncFloat* CallDoFloat(float durTime, float start, float end, const FHandleFloat_Single& OutOnComplete, const FHandleFloat_Single& OutOnUpdate);

	UPROPERTY(BlueprintAssignable)
		FHandleFloat OnComplete;

	UPROPERTY(BlueprintAssignable)
		FHandleFloat OnUpdate;


	float StartValue;
	float EndValue;
protected:
	virtual void UpdateFunc(float deltaTime) override;
};
