// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DotweenBase.h"
#include "DoTweenVector3.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleFVector3, FVector, curValue,UDoTweenBase*, base);
UCLASS( )
class MESPRODUCT_API UDoTweenVector3 : public UDoTweenBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UDoTweenVector3* DoVector3(float durTime, FVector start, FVector FVector);

	UPROPERTY(BlueprintAssignable)
		FHandleFVector3 OnComplete;

	UPROPERTY(BlueprintAssignable)
		FHandleFVector3 OnUpdate;

	FVector StartValue;
	FVector EndValue;
protected:
	virtual void UpdateFunc(float deltaTime) override;
};
