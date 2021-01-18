// Fill out your copyright notice in the Description page of Project Settings.


#include "TestBlueprintAsyncActionBase.h"
FDeleGateTest CurDelegate = FDeleGateTest();
UTestBlueprintAsyncActionBase* UTestBlueprintAsyncActionBase::AsyncPrint()
{
	UTestBlueprintAsyncActionBase* BlueprintNode = NewObject<UTestBlueprintAsyncActionBase>();
	BlueprintNode->Start();
	return BlueprintNode;
}

FString UTestBlueprintAsyncActionBase::ExeDelegate()
{
	UTestBlueprintAsyncActionBase* BlueprintNode = NewObject<UTestBlueprintAsyncActionBase>();
	BlueprintNode->TestDeleGate();
	FString CurString = "AAA";
	CurDelegate.ExecuteIfBound(CurString);
	return CurString;
}

void UTestBlueprintAsyncActionBase::TestDeleGate()
{
	CurDelegate.BindUObject(this, &UTestBlueprintAsyncActionBase::DeleEvent);
}


void UTestBlueprintAsyncActionBase::DeleEvent(FString &String)
{
	String += "TestSuccess";
	
}

void UTestBlueprintAsyncActionBase::Start()
{
	Async(EAsyncExecution::ThreadPool, [&]() {

		FPlatformProcess::Sleep(1.0f);
		int32 result = FMath::RandRange(1, 100);

		FString CurString = FString::FromInt(result);
		if (result > 50)
		{
			this->OnSuccess.Broadcast(CurString);
		}
		else
		{
			this->OnFail.Broadcast(CurString);
		}

		});
}
