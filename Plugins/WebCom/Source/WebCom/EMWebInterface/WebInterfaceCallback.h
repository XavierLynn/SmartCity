// Copyright 2021 x.Lin. All Rights Reserved
#pragma once
#include "JsonLibrary.h"
#include "WebInterfaceCallback.generated.h"

class UComWebBrowser;

USTRUCT(BlueprintType)
struct WEBCOM_API FWebInterfaceCallback
{
	friend class UComWebBrowser;
	friend class UWebInterfaceObject;

	GENERATED_USTRUCT_BODY()

	FWebInterfaceCallback();

protected:
	
	FWebInterfaceCallback( TWeakObjectPtr<UComWebBrowser> Interface, const FString& Callback );

public:
	
	bool IsValid() const;
	void Call( const FJsonLibraryValue& Data );

private:

	FString MyCallback;
	TWeakObjectPtr<UComWebBrowser> MyInterface;
};
