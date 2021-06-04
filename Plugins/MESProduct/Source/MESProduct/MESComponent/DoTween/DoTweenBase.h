// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DoTweenBase.generated.h"

UCLASS()
class MESPRODUCT_API UDoTweenBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
			void StopUpdate();

protected:

	void Init(float durTime);
	virtual void UpdateFunc(float deltaTime);
	
	virtual void SetReadyToDestroy() override;
	float TotalTime;
	float CurTime;
	int UpdateKey;
	bool Finished;
};
