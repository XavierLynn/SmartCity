// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebBrowser.h"
#include "MESWebBrowser.generated.h"

/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESWebBrowser : public UWebBrowser
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)

		void BindToObject(const FString& Name, UObject* Object, bool bIsPermanent = true);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MES×é¼þÊôÐÔ", Meta = (DisplayName = "CanConstruct"))
		bool CanConstruct = true;
	UFUNCTION(BlueprintPure, Category = "MESFunc")
			bool IsLoaded();

public:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

protected:
	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
};
