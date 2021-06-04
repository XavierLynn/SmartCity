// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageBox.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMessageBox : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
		void Init(FString Message);

private:
	void OnButtonSure();
};
