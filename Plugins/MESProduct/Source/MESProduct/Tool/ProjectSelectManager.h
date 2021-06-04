// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectSelectManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMESOnViewportResized, int, X, int, Y);

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UProjectSelectManager : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
		bool CheckUrl(FString InUrl);

	void OnViewportResized(class FViewport* Viewport, uint32 Value);

	UPROPERTY(BlueprintAssignable)
		FMESOnViewportResized OnMESViewportResized;
};
