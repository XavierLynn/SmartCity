// Copyright XuLin, Inc. All Rights Reserved..

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "TestBlueprintAsyncActionBase.generated.h"



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAsyncAction, FString, ReturnString);
DECLARE_DELEGATE_OneParam(FDeleGateTest, FString&);

UCLASS()
class UECHARTS_API UTestBlueprintAsyncActionBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
		FAsyncAction OnSuccess;
	UPROPERTY(BlueprintAssignable)
		FAsyncAction OnFail;
public:
	UFUNCTION(BlueprintCallable,meta = (BlueprintInternalUseOnly = "true"),Category = "TestBlueprintAsyncActionBase")
		static UTestBlueprintAsyncActionBase* AsyncPrint();
	UFUNCTION(BlueprintCallable)
		static FString ExeDelegate();
private:
	void TestDeleGate();
	void DeleEvent(FString &String);
	void Start();
	
};
