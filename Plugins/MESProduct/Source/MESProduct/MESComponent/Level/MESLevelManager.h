// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MESLevelManager.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FLevelShownDelegate, float, Percent);
DECLARE_DYNAMIC_DELEGATE_OneParam(FLevelLoadDelegate, int, RemainLevelCount);

DECLARE_DYNAMIC_DELEGATE(FLevelChangeStartDynamicDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLevelChangeStartDynamicMultiDelegate);


UCLASS()
class MESPRODUCT_API AMESLevelManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMESLevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "MESUtil | Level", meta = (WorldContext = "WorldContextObject"))
		static void ShowLevel(UObject* WorldContextObject, TArray<FString> LevelPaths, FLevelShownDelegate Callback, bool isCanCache);

	UFUNCTION(BlueprintCallable, Category = "MESUtil | Level", meta = (WorldContext = "WorldContextObject"))
		static void SetLevelVisible(UObject* WorldContextObject, TArray<FString> LevelPaths, bool IsVisble = true);

	UFUNCTION(BlueprintCallable, Category = "MESUtil | Level", meta = (WorldContext = "WorldContextObject"))
		static void BindOnLevelChangeStart(UObject* WorldContextObject, FLevelChangeStartDynamicDelegate Callback);

	UFUNCTION(BlueprintCallable, Category = "MESUtil | Level", meta = (WorldContext = "WorldContextObject"))
		static void UnbindOnLevelChangeStart(UObject* WorldContextObject, FLevelChangeStartDynamicDelegate Callback);

	void RealShow();

private:
	static void HandlePath(TArray<FString>& LevelPaths);
	static AMESLevelManager* Instance;
	int LevelCountToLoad;
	int LevelCountToHide;
	bool CanCache = true;
	float MaxTaskCont;
	bool CanCallBack = false;
	UPROPERTY()
		TMap<FString, class ULevelStreamingDynamic*> ShowedLevels; // key: LevelPath
	TArray<FString> LastShowedLevel;
	TArray<FString> WillShowedLevel;
	FLevelShownDelegate OnLevelShownCallback;
	FLevelLoadDelegate OnLevelLoadedCallback;

	static FLevelChangeStartDynamicMultiDelegate OnLevelChangeStartCallbacks;

	UFUNCTION()
		void OnLevelShowned();
	UFUNCTION()
		void OnLevelLoaded();

	UFUNCTION()
		void OnLevelHidened();
};
