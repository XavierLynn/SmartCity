// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Async/AsyncWork.h"
#include "VideoComp.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UVideoComp : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
		void AsyncPlayVideo();

	UFUNCTION(BlueprintImplementableEvent)
		void OnAsyncPlayVideo();
};

class VideoAutoDeleteAsyncTask :public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<VideoAutoDeleteAsyncTask>;

	UVideoComp* Target;

	VideoAutoDeleteAsyncTask(UVideoComp* InObject)
		:Target(InObject)
	{

	}

	void DoWork()
	{
		if (Target)
		{
			Target->OnAsyncPlayVideo();
		}
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(VideoAutoDeleteAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};