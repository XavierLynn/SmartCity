// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DotweenBase.h"
#include "DoTweenVector4.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHandleFVector4, FVector4, curValue, UDoTweenBase*, base);
UCLASS( )
class MESPRODUCT_API UDoTweenVector4 : public UDoTweenBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UDoTweenVector4* DoVector4(float durTime, FVector4 start, FVector4 end);

	UPROPERTY(BlueprintAssignable)
		FHandleFVector4 OnComplete;

	UPROPERTY(BlueprintAssignable)
		FHandleFVector4 OnUpdate;

	FVector4 StartValue;
	FVector4 EndValue;
protected:
	virtual void UpdateFunc(float deltaTime) override;
};
