// Copyright 2021 x.Lin. All Rights Reserved
#pragma once
#include "JsonLibrary.h"
#include "WebInterfaceObject.generated.h"

class UComWebBrowser;

UCLASS()
class WEBCOM_API UWebInterfaceObject : public UObject
{
	friend class UComWebBrowser;

	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "WEBCOM")
	void Broadcast( const FString& Name, const FString& Data, const FString& Callback );

private:

	TWeakObjectPtr<UComWebBrowser> MyInterface;
};
