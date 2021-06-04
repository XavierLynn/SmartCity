// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TreeView.h"
#include "MESTreeView.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESTreeView : public UTreeView
{
	GENERATED_BODY()


public:
	UMESTreeView(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void OnItemClickedInternal(UObject* ListItem) override;

	virtual void OnWidgetRebuilt() override;

public:
	UFUNCTION(BlueprintCallable)
		void ExpandSelectItem(UObject* ListItem);

	UFUNCTION(BlueprintCallable)
		void ExpandAllItems();

	UFUNCTION(BlueprintCallable)
		bool DoesItemHaveChildren(UObject* Item);

	UFUNCTION(BlueprintCallable)
		void SetSingleExpandedItem(UObject* Item);
};
