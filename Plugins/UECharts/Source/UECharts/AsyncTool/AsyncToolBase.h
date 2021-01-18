// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncToolBase.generated.h"

/**
 * 
 */


UCLASS()
class UECHARTS_API UAsyncToolBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AsyncTool")
		void StopUpdate();
protected:
	void Init(float durTime);
	virtual void UpdateFunc(float deltaTime);

protected:
	

	virtual void SetReadyToDestroy() override;
	float TotalTime;
	float CurTime;
	int UpdateKey;
	bool Finished;
};
