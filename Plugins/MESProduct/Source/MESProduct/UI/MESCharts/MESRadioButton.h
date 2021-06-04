// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MESRadioButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectCallBack, bool, IsSelected);
/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESRadioButton : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "MES组件属性", Meta = (DisplayName = "IsSelect"))
		bool IsSelect = false;
	
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
			void OnSelect();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
			void OnUnSelect();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void OnInit();

	UPROPERTY(BlueprintAssignable, Category = "MES组件属性", Meta = (DisplayName = "SelectedEvent"))
		FSelectCallBack SelectedEvent;

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
private:
	static TMap<uint32, TArray<UMESRadioButton*>> DicButton;
};
