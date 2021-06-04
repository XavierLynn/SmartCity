// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct\MESComponent\ComponetBase\MESActorBase.h"
#include "MESProduct\MESComponent\ComponetBase\GisRootBase.h"
#include "MESProduct\MESComponent\ComponetBase\PointActorBase.h"
#include "MESProduct\MESComponent\ComponetBase\MESComponetBase.h"
#include "MESPointComponetBase.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API AMESPointComponetBase : public AMESActorBase
{
	GENERATED_BODY()
public:
	AMESPointComponetBase();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "打点特效"))
		TSubclassOf<APointActorBase> PointActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "打点Tips"))
		TSubclassOf<UMESComponetBase> UITips;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "坐标系"))
		EGISType GisType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "缩放"))
		float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "Tips互斥"))
		bool SingleTips;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "地面之上"))
		bool OnBackGround;

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetFinishForPoint();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void SetCommonDataInfo(FActorBaseData data);

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		static void ShowPoints(FString PointCompnetName);

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		static void HidePoints(FString PointCompnetName);

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		void SetMainTrans(FTransform trans);

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		APointActorBase* GetFirstPoint();

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		APointActorBase* GetNextPoint(APointActorBase* curPoint);

	UFUNCTION(BlueprintCallable, Category = "MESUtil")
		void UpdateMainPointTrans();
		
    UFUNCTION(BlueprintCallable, Category = "MESUtil")
	    void ClearPoint();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginPlay() override;

	virtual void OnReceiveData() override;
	
	TArray<APointActorBase*> Points;

	static TMap<FString, AMESPointComponetBase*> PointDic;

};
