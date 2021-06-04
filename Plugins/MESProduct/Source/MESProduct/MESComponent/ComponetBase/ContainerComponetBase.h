// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/MESComponent/ComponetBase/MESComponetBase.h"
#include "ContainerComponetBase.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UContainerComponetBase : public UMESComponetBase
{
	GENERATED_BODY()
	

public:
	static TArray<UContainerComponetBase*> ConstainStackInMoving;

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void AddInMovingStack();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		void RemoveInMovingStack();

	UFUNCTION(BlueprintCallable, Category = "MESFunc")
		static UContainerComponetBase* GetTopContainer();
public:
	virtual void NativeDestruct() override;

};
