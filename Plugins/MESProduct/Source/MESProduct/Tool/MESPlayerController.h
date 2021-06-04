// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Misc/TextFilter.h"
#include "MESPlayerController.generated.h"


DECLARE_DYNAMIC_DELEGATE(FSetFilterDelegate);

typedef TTextFilter<const FString&> MESStringFilter;

USTRUCT(BlueprintType)
struct FMESStringFilter
{
	GENERATED_BODY()
		
		TSharedPtr<MESStringFilter> Filter;

	UPROPERTY(BlueprintReadWrite)
		FSetFilterDelegate Callback;

	UPROPERTY(BlueprintReadWrite)
		FString FilterName;
};


/**
 * 
 */
UCLASS()
class MESPRODUCT_API AMESPlayerController : public APlayerController
{
	GENERATED_BODY()


	AMESPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
		void SetDPIScaleFactor(float Value);

	UFUNCTION(BlueprintCallable)
		static void SetStringFilter(FString FilterName, FString FilterString, FSetFilterDelegate Callback);
	UFUNCTION(BlueprintCallable)
		static bool TestFilter(FString FilterName, FString TestString);

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		class UHttpNetComponent* HttpComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		class URabbitMqClient* RabbitMqComponent;

private:
	static TMap<FString, FMESStringFilter> StringFilterMap;
	static MESStringFilter::FItemToStringArray ItemToStringDelegate;

	void ItemToString(const FString& SourceData, TArray<FString>& OutSearchStrings);
};
