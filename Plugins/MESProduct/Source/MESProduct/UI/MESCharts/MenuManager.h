// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MESProduct/MESComponent/ComponetBase/MESComponetBase.h"
#include "MenuManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMenuEvent, int, menuID,FString,MenuName);
USTRUCT(BlueprintType)
struct FMenuDataLv2
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int menuId;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString menuName;
	
	long menuLevel;
};

USTRUCT(BlueprintType)
struct FMenuDataLv1
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int menuId;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString menuName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMenuDataLv2> TwoLevelMenu;
	
	long menuLevel;
	
};

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMenuManager : public UMESComponetBase
{
	GENERATED_BODY()

public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "定制属性", Meta = (DisplayName = "菜单数据"))
		TArray<FMenuDataLv1> MenuData;

		UPROPERTY(BlueprintAssignable, Category = "定制属性", Meta = (DisplayName = "菜单事件"))
			FMenuEvent MenuEvent;

public:
	UFUNCTION(BlueprintCallable, Category = "MESFunc")
			void ChangeMenuTo(int MenuID,FString MenuName);

	
	UFUNCTION(BlueprintCallable, Category = "MESUtil", Meta = (DisplayName = "MenuManager.Instance"))
		static UMenuManager* Instance();

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface
private:
	static UMenuManager* mInstance;
};

