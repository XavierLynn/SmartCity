// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DotweenBase.h"
#include "DoTweenVector2.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleFVector2D, FVector2D, curValue, UDoTweenBase*, base);
UCLASS( )
class MESPRODUCT_API UDoTweenVector2 : public UDoTweenBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UDoTweenVector2* DoVector2(float durTime, FVector2D start, FVector2D end);

	UPROPERTY(BlueprintAssignable)
		FHandleFVector2D OnComplete;

	UPROPERTY(BlueprintAssignable)
		FHandleFVector2D OnUpdate;

	FVector2D StartValue;
	FVector2D EndValue;
protected:
	virtual void UpdateFunc(float deltaTime) override;
};
