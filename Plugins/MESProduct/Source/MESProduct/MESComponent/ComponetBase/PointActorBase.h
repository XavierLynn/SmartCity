// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PointActorBase.generated.h"

class AMESPointComponetBase;

UCLASS()
class MESPRODUCT_API APointActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	APointActorBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "Key"))
		FString Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "TipsName"))
		FString TipsName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性",Meta = (DisplayName = "打点数据"))
		TArray<FString> PointData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES组件属性", Meta = (DisplayName = "生成打点的组件"))
		AMESPointComponetBase* PointMain;

	UFUNCTION(BlueprintImplementableEvent, Category = "MESFunc")
			void OnDataInit();

	UFUNCTION(BlueprintImplementableEvent, Category = "MESFunc")
			void OnShow(bool IsMasked);		

	UFUNCTION(BlueprintImplementableEvent, Category = "MESFunc")
			void OnFade();//逐渐消失

protected:
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

};
