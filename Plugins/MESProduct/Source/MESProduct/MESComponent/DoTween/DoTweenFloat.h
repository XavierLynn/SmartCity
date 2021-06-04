// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DotweenBase.h"
#include "DoTweenFloat.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FHandleFloat_Single, float, curValue, int, key);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleFloat, float, curValue, UDoTweenBase*,base);
UCLASS( )
class MESPRODUCT_API UDoTweenFloat : public UDoTweenBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UDoTweenFloat* DoFloat(float durTime, float start, float end);

	UFUNCTION(BlueprintCallable)
	static UDoTweenFloat* CallDoFloat(float durTime, float start, float end, const FHandleFloat_Single&OutOnComplete, const FHandleFloat_Single& OutOnUpdate);

	UPROPERTY(BlueprintAssignable)
		FHandleFloat OnComplete;

	UPROPERTY(BlueprintAssignable)
		FHandleFloat OnUpdate;

	float StartValue;
	float EndValue;
protected:
	virtual void UpdateFunc(float deltaTime) override;
};
